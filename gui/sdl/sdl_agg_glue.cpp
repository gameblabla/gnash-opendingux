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


/* alekmaul's scaler taken from mame4all */
static void bitmap_scale(uint32_t startx, uint32_t starty, uint32_t viswidth, uint32_t visheight, uint32_t newwidth, uint32_t newheight,uint32_t pitchsrc,uint32_t pitchdest, uint16_t* src, uint16_t* dst)
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

namespace gnash
{

SdlAggGlue::SdlAggGlue()
	:
_sdl_surface(nullptr),
_offscreenbuf(nullptr),
_screen(nullptr),
_agg_renderer(nullptr)
{
//    GNASH_REPORT_FUNCTION;
}

SdlAggGlue::~SdlAggGlue()
{
//    GNASH_REPORT_FUNCTION;
    SDL_FreeSurface(_sdl_surface);
   // SDL_FreeSurface(_screen);
    delete [] _offscreenbuf;
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

    assert(_bpp % 8 == 0);
    
    printf("Internal width, height %d %d\n", width, height);

    //_screen = SDL_SetVideoMode(width, height, _bpp, sdl_flags | SDL_SWSURFACE);
    rl_screen = SDL_SetVideoMode(320, 240, _bpp, SDL_HWSURFACE);

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

    _offscreenbuf = new unsigned char[bufsize];

    log_debug (_("SDL-AGG: %i byte offscreen buffer allocated"), bufsize);


    // Only the AGG renderer has the function init_buffer, which is *not* part of
    // the renderer api. It allows us to change the renderers movie size (and buffer
    // address) during run-time.
    Renderer_agg_base * renderer =
      static_cast<Renderer_agg_base *>(_agg_renderer);
    renderer->init_buffer(_offscreenbuf, bufsize, width, height,
      width*((_bpp+7)/8));


    _sdl_surface = SDL_CreateRGBSurfaceFrom((void *) _offscreenbuf, width, height,
                                           _bpp, stride, rmask, gmask, bmask, amask);
    assert(_sdl_surface);
    
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

void
SdlAggGlue::render(int minx, int miny, int maxx, int maxy)
{
    // Update only the invalidated rectangle
    /*SDL_Rect clip = { static_cast<Sint16>(minx),
        static_cast<Sint16>(miny),
        static_cast<Uint16>(maxx - minx),
        static_cast<Uint16>(maxy - miny)};
    SDL_SetClipRect(_screen, &clip);*/
   // SDL_BlitSurface(_sdl_surface, nullptr, _screen, nullptr);
   bitmap_scale(0, 0, _sdl_surface->w, _sdl_surface->h, rl_screen->w, rl_screen->h, _sdl_surface->w,0, (uint16_t*)_sdl_surface->pixels, (uint16_t*)rl_screen->pixels);
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
    //SDL_UpdateRect(_screen, clip.x, clip.y, clip.w, clip.h);
}

} // namespace gnash
