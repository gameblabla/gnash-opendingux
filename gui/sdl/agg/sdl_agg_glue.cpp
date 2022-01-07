// sdl_agg_glue.cpp:  Glue between SDL and Anti-Grain Geometry, for Gnash.
//
//   Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010,
//   2011 Free Software Foundation, Inc
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "sdl_agg_glue.h"
#include "log.h"
#include "Renderer.h"
#include "Renderer_agg.h"
#include <cerrno>
#include <ostream>

using namespace std;

static SDL_Surface* rl_screen, *cursor_sdl;
SDL_RWops *rw;
int internal_width = 0, internal_height = 0, hw_width = 0, hw_height = 0;
extern int mouse_mode;
extern int mouse_x, mouse_y;

#ifdef OPENDINGUX
#ifndef SDL_TRIPLEBUF
#define SDL_TRIPLEBUF SDL_DOUBLEBUF
#endif
int toscaleup = 0;
#endif


/* alekmaul's scaler taken from mame4all */
static inline void bitmap_scale(uint32_t startx, uint32_t starty, uint32_t viswidth, uint32_t visheight, uint32_t newwidth, uint32_t newheight,uint32_t pitchsrc,uint32_t pitchdest, uint16_t* src, uint16_t* dst)
{
    uint32_t W,H,ix,iy,x,y;
    x=startx<<16;
    y=starty<<16;
    W=newwidth;
    H=newheight;
    ix=(viswidth<<16)/W;
    iy=(visheight<<16)/H;

    do 
    {
        uint16_t* buffer_mem=&src[(y>>16)*pitchsrc];
        W=newwidth; x=startx<<16;
        do 
        {
            *dst++=buffer_mem[x>>16];
            x+=ix;
        } while (--W);
        dst+=pitchdest;
        y+=iy;
    } while (--H);
}

void Exit_App()
{
    if (rl_screen != nullptr)
    {
		SDL_FreeSurface(rl_screen);
		rl_screen = rl_screen;
	}
	if (cursor_sdl != nullptr)
	{
		SDL_FreeSurface(cursor_sdl);
		cursor_sdl = nullptr;
	}
	//SDL_Quit();
}

namespace gnash
{

SdlAggGlue::SdlAggGlue()
	:
//_sdl_surface(nullptr),
_offscreenbuf(nullptr),
_screen(nullptr),
_agg_renderer(nullptr)
{
//    GNASH_REPORT_FUNCTION;
}

SdlAggGlue::~SdlAggGlue()
{
//    GNASH_REPORT_FUNCTION;
    //SDL_FreeSurface(_sdl_surface);
	//SDL_FreeSurface(_screen);
	#ifdef OPENDINGUX
    if (_sdl_surface != nullptr)
    {
		SDL_FreeSurface(_sdl_surface);
		_sdl_surface = nullptr;
	}
	#endif
    if (rl_screen != nullptr)
    {
		SDL_FreeSurface(rl_screen);
		rl_screen = nullptr;
	}
	if (cursor_sdl != nullptr)
	{
		SDL_FreeSurface(cursor_sdl);
		cursor_sdl = nullptr;
	}
}

bool
SdlAggGlue::init(int /*argc*/, char*** /*argv*/)
{
//    GNASH_REPORT_FUNCTION;

    return true;
}


Renderer*
SdlAggGlue::createRenderHandler(int bpp)
{
//    GNASH_REPORT_FUNCTION;

    _bpp = 16;

    switch (_bpp) {
      case 32:
        _agg_renderer = create_Renderer_agg("RGBA32");
        break;
      case 24:
        _agg_renderer = create_Renderer_agg("RGB24");
        break;
      case 16:
        _agg_renderer = create_Renderer_agg("RGBA16");
        break;
      default:
        log_error (_("AGG's bit depth must be 16, 24 or 32 bits, not %d."), _bpp);
        abort();
    }
    return _agg_renderer;
}


bool
SdlAggGlue::prepDrawingArea(int width, int height, std::uint32_t sdl_flags)
{
    int depth_bytes = _bpp / 8;  // TODO: <Udo> is this correct? Gives 1 for 15 bit modes!

    //assert(_bpp % 8 == 0);
    
    printf("Internal width, height %d %d\n", width, height);
    #if !defined(NOHIDEMOUSE)
    SDL_ShowCursor(SDL_DISABLE);
    #endif
	_sdl_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, _bpp, 0,0,0,0);
	#ifdef OPENDINGUX
	toscaleup = 0;
	rl_screen = SDL_SetVideoMode(width, height, _bpp, SDL_HWSURFACE);
	if (!rl_screen)
	{
		rl_screen = SDL_SetVideoMode(width, height + 1, _bpp, SDL_HWSURFACE);
		if (!rl_screen)
		{
			toscaleup = 1;
			rl_screen = SDL_SetVideoMode(0, 0, _bpp, SDL_HWSURFACE);
		}
	}
	#else
    //_screen = SDL_SetVideoMode(width, height, _bpp, sdl_flags | SDL_SWSURFACE);
    rl_screen = SDL_SetVideoMode(0, 0, _bpp, SDL_HWSURFACE);
    #endif

	SDL_Surface* tmp = SDL_LoadBMP("cursor.bmp"); /* Automatically frees the RWops struct for us */
	SDL_SetColorKey(tmp, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(tmp->format, 255, 0, 255));
	cursor_sdl = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);

    internal_width = width;
    internal_height = height;
    
    hw_width = rl_screen->w;
    hw_height = rl_screen->h;

    
    //_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, _bpp, 0,0,0,0);

   /* if (!_screen) {
        log_error (_("SDL_SetVideoMode() failed for SdlAggGlue."));
        exit(EXIT_FAILURE);
    }*/

    int stride = width * depth_bytes;

    std::uint32_t rmask, gmask, bmask, amask;

    switch(_bpp) {
      case 32: // RGBA32
        rmask = 0xFF;
        gmask = 0xFF << 8;
        bmask = 0xFF << 16;
        amask = 0xFF << 24;
        break;
      case 24: // RGB24
        rmask = 0xFF;
        gmask = 0xFF << 8;
        bmask = 0xFF << 16;
        amask = 0;
        break;
      case 16: // RGB565: 5 bits for red, 6 bits for green, and 5 bits for blue
        rmask = 0x1F << 11;
        gmask = 0x3F << 5;
        bmask = 0x1F;
        amask = 0;
        break;
      default:
        abort();
    }

	#define CHUNK_SIZE (100 * 100 * depth_bytes)

	int bufsize = static_cast<int>(width * height * depth_bytes / CHUNK_SIZE + 1) * CHUNK_SIZE;
	_offscreenbuf = (unsigned char*)_sdl_surface->pixels;
	Renderer_agg_base * renderer =
	static_cast<Renderer_agg_base *>(_agg_renderer);
	renderer->init_buffer(_offscreenbuf, bufsize, width, height,
	width*((_bpp+7)/8));

	/*
    _sdl_surface = SDL_CreateRGBSurfaceFrom((void *) _offscreenbuf, width, height,
                                           _bpp, stride, rmask, gmask, bmask, amask);
	*/
    //assert(_sdl_surface);
    
    _validbounds.setTo(0, 0, width-1, height-1);
    
    return true;
}

/// Modified from fb_gui
void
SdlAggGlue::setInvalidatedRegions(const InvalidatedRanges& ranges)
{
    _agg_renderer->set_invalidated_regions(ranges);
    _drawbounds.clear();
    
    for (unsigned int rno=0; rno<ranges.size(); rno++) {
        geometry::Range2d<int> bounds = Intersection(
            // twips changed to pixels here
            _agg_renderer->world_to_pixel(ranges.getRange(rno)),
            _validbounds);
            
        // it may happen that a particular range is out of the screen, which
        // will lead to bounds==null.
        if (bounds.isNull()) continue;
        _drawbounds.push_back(bounds);
    }
}

void
SdlAggGlue::render()
{
    if (_drawbounds.empty()) return; // nothing to do..
    
    for (unsigned int bno=0; bno < _drawbounds.size(); bno++) {
        geometry::Range2d<int>& bounds = _drawbounds[bno];
        render(bounds.getMinX(), bounds.getMinY(),
            bounds.getMaxX(), bounds.getMaxY() );
    }
}

int old_x, old_y;
int firstime = 0;
void
SdlAggGlue::render(int minx, int miny, int maxx, int maxy)
{
    // Update only the invalidated rectangle
   // SDL_BlitSurface(_sdl_surface, nullptr, _screen, nullptr);
   
   #ifdef OPENDINGUX
	SDL_Rect clip = { minx, miny, maxx - minx, maxy - miny};
	SDL_Rect position;
	if (toscaleup == 1)
	{
	   bitmap_scale(0, 0, internal_width, internal_height, rl_screen->w, rl_screen->h, internal_width,0, (uint16_t*)_offscreenbuf, (uint16_t*)rl_screen->pixels);
	   //SDL_SoftStretch(_sdl_surface, NULL, rl_screen, NULL);
	   if (mouse_mode == 1)
	   {
			position.x = mouse_x;
			position.y = mouse_y;
			SDL_BlitSurface(cursor_sdl, nullptr, rl_screen, &position);
		}
		SDL_UpdateRect(rl_screen, 0, 0, 0, 0);
	}
	else
	{
		memcpy(rl_screen->pixels, _sdl_surface->pixels, (rl_screen->w * rl_screen->h) *2);
		if (mouse_mode == 1)
		{
			SDL_SetClipRect(rl_screen, NULL);
			position.x = mouse_x;
			position.y = mouse_y;
			SDL_BlitSurface(cursor_sdl, nullptr, rl_screen, &position);
			SDL_UpdateRect(rl_screen, 0, 0, 0, 0);
		}
		else
		{
			SDL_SetClipRect(rl_screen, &clip);
			SDL_UpdateRect(rl_screen, clip.x, clip.y, clip.w, clip.h);
		}
	}
   #else
   bitmap_scale(0, 0, internal_width, internal_height, rl_screen->w, rl_screen->h, internal_width,0, (uint16_t*)_offscreenbuf, (uint16_t*)rl_screen->pixels);
   //SDL_SoftStretch(_sdl_surface, NULL, rl_screen, NULL);
   if (mouse_mode == 1)
   {
		SDL_Rect position;
		position.x = mouse_x;
		position.y = mouse_y;
		if (cursor_sdl)
		SDL_BlitSurface(cursor_sdl, nullptr, rl_screen, &position);
		else
		{
			position.w = 4;
			position.h = 4;
			SDL_FillRect(rl_screen, &position, 512);
		}
	}
	SDL_Flip(rl_screen);
	#endif
    //SDL_UpdateRect(_screen, clip.x, clip.y, clip.w, clip.h);
}

} // namespace gnash
