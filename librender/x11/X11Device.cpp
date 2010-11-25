//
//   Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010 Free Software
//   Foundation, Inc
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

#ifdef HAVE_CONFIG_H
#include "gnashconfig.h"
#endif

#include <iostream>
#include <cerrno>
#include <exception>
#include <sstream>

#include "log.h"
#include "GnashException.h"

#ifdef HAVE_GTK2
#include "gdk/gdkx.h"
#endif

#ifdef HAVE_X11_X_H
# include <X11/X.h>
# include <X11/Xlib.h>
# include <X11/Xutil.h>
#include <X11/keysym.h>
#else
# error "This file needs X11"
#endif

#include "X11Device.h"
#include "GnashDevice.h"

namespace gnash {

namespace renderer {

namespace x11 {
    
// The debug log used by all the gnash libraries.
static LogFile& dbglogfile = LogFile::getDefaultInstance();

// FIXME: this font name shouldn't be hardcoded!
const char *FONT = "/usr/share/fonts/truetype/freefont/FreeSerif.ttf";

X11Device::X11Device()
    : _display(0),
      _screennum(0),
      _window(0),
      _visual(0),
      _screen(0),
      _depth(0),
      _vinfo(0)
{
    GNASH_REPORT_FUNCTION;
    dbglogfile.setVerbosity();
}

X11Device::~X11Device()
{
    GNASH_REPORT_FUNCTION;
    if (_display) {
        if (_window) {
            XDestroyWindow(_display, _window);
        }
        XCloseDisplay(_display);
    }
    XFree(_vinfo);
}

bool
X11Device::initDevice(int argc, char *argv[])
{
    GNASH_REPORT_FUNCTION;

#ifdef HAVE_GTK2_XX
    // As gdk_init() wants the command line arguments, we have to create
    // fake ones, as we don't care about the X11 options at this point.
    gdk_init(&argc, &argv);

#if 0
    GdkVisual* wvisual = gdk_drawable_get_visual(_drawing_area->window);

    GdkImage* tmpimage = gdk_image_new (GDK_IMAGE_FASTEST, wvisual, 1, 1);

    const GdkVisual* visual = tmpimage->visual;

    // FIXME: we use bpp instead of depth, because depth doesn't appear to
    // include the padding byte(s) the GdkImage actually has.
    const char *pixelformat = agg_detect_pixel_format(
        visual->red_shift, visual->red_prec,
        visual->green_shift, visual->green_prec,
        visual->blue_shift, visual->blue_prec,
        tmpimage->bpp * 8);

    gdk_image_destroy(tmpimage);
#endif  // end of 0
#else
    char *dpyName = NULL;
    int num_visuals;
 
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-display") == 0) {
            dpyName = argv[i+1];
            i++;
        }
    }

    _display = XOpenDisplay(dpyName);
    if (!_display) {
        log_error("couldn't open X11 display!");
        return false;
    }

    _window = XDefaultRootWindow(_display);
    _screennum = XDefaultScreen(_display);

    _depth = DefaultDepth(_display, _screennum);
    _colormap = DefaultColormap(_display, _screennum);

    _visual = XDefaultVisual(_display, _screennum);

    _screen = DefaultScreenOfDisplay(_display);

    VisualID vid = XVisualIDFromVisual(_visual);
    
    XVisualInfo visTemplate;
    visTemplate.visualid = vid;
    
    _vinfo = XGetVisualInfo(_display, VisualIDMask, &visTemplate, &num_visuals);
    // std::cerr << "Num Visuals: " << num_visuals << std::endl;
    if (!_vinfo) {
         log_error("Error: couldn't get X visual\n");
         exit(1);
    }

    // int re = visInfo[0].bits_per_rgb;
    
    XWindowAttributes gattr;
    XGetWindowAttributes(_display, _window, &gattr);
    
    // std::cerr << "Width: " << gattr.backing_store << std::endl;
    // std::cerr << "Width: " << gattr.depth << std::endl;

#if 0
    const char *name = "Foo";
    XSetWindowAttributes attr;
    unsigned long mask;    
    Window win;
    int width, height;
    int x, y;
    
    // window attributes
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(_display, root, visInfo->visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
    
    win = XCreateWindow(_display, root, 0, 0, width, height,
                         0, visInfo->depth, InputOutput,
                         visInfo->visual, mask, &attr );
    
#endif

#if 0
    // set hints and properties
    XSizeHints sizehints;
    sizehints.x = x;
    sizehints.y = y;
    sizehints.width  = width;
    sizehints.height = height;
    sizehints.flags = USSize | USPosition;
    XSetNormalHints(_display, win, &sizehints);
    XSetStandardProperties(_display, win, name, name,
                           None, (char **)NULL, 0, sizehints);
#endif
    
   // XFree(visInfo);
#endif
    
    return true;
}

// Initialize X11 Window layer
bool
X11Device::attachWindow(GnashDevice::native_window_t window)
{
    GNASH_REPORT_FUNCTION;

    _window = static_cast<Window>(window);

    return true;
}
    

// Return a string with the error code as text, instead of a numeric value
const char *
X11Device::getErrorString(int error)
{
    static char msg[80];
    
    if (_display) {
        XGetErrorText(_display, error, msg, 80);
    } else {
        log_error("The default Display is not set!");
    }

    return msg;
}

// Create an X11 window to render in. This is only used by testing
void
X11Device::createWindow(const char *name, int x, int y, int width, int height)
{
    GNASH_REPORT_FUNCTION;

    if (!_display) {
        log_error("No Display device set!");
        return;
    }
    
    if (!_window) {
        log_error("No drawable window set!");
        return;
    }

    XSetWindowAttributes attr;
    unsigned long mask;
    Window root;
    XVisualInfo visTemplate;
    int num_visuals;

    // window attributes
    attr.background_pixel = 0;
    attr.border_pixel = 0;
    attr.colormap = XCreateColormap(_display, _window, _vinfo->visual, AllocNone);
    attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
    mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
    
    _window = XCreateWindow(_display, _window, 0, 0, width, width,
                        0, _vinfo->depth, InputOutput,
                        _vinfo->visual, mask, &attr);
    
    // set hints and properties
    XSizeHints sizehints;
    sizehints.x = x;
    sizehints.y = y;
    sizehints.width  = width;
    sizehints.height = height;
    sizehints.flags = USSize | USPosition;
    XSetNormalHints(_display, _window, &sizehints);
    XSetStandardProperties(_display, _window, name, name, None, (char **)NULL,
                           0, &sizehints);

    XMapWindow(_display, _window);
//    reshape(width, height);


}

void
X11Device::event_loop(size_t passes)
{
    while (passes--) {
        int redraw = 0;
        XEvent event;
        int width, height;
        
        reshape_func reshape = 0;
        key_func     keyPress = 0;
        draw_func    draw = 0;
        
        XNextEvent(_display, &event);
        
        switch (event.type) {
          case Expose:
              redraw = 1;
              break;
          case ConfigureNotify:
              if (reshape) {
                  width = event.xconfigure.width;
                  height = event.xconfigure.height;
                  reshape(event.xconfigure.width, event.xconfigure.height);
              }
              break;
          case KeyPress:
          {
              char buffer[10];
              int r, code;
              code = XLookupKeysym(&event.xkey, 0);
              if (!keyPress || !keyPress(code)) {
                  r = XLookupString(&event.xkey, buffer, sizeof(buffer),
                                    NULL, NULL);
                  if (buffer[0] == 27) {
                      // escape
                      return;
                  } else {
                      std::cerr << buffer;
                  }
              }
          }
          redraw = 1;
          break;
          default:
              ; // no-op
        }
        
        if (redraw) {
//            draw();
//            eglSwapBuffers(egl_dpy, egl_surf);
        }
    } // end of while passes
}

} // namespace x11
} // namespace renderer
} // namespace gnash

// local Variables:
// mode: C++
// indent-tabs-mode: nil
// End:
