/* gnashconfig.h.  Generated from gnashconfig.h.in by configure.  */
/* gnashconfig.h.in.  Generated from configure.ac by autoheader.  */

/* ABS Resolution field */
#define ABSINFO_RESOLUTION 1

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* this is an AmigaOS4 platform */
/* #undef AMIGAOS4_HOST */

/* This is an Android build */
/* #undef ANDROID */

/* this is an Android platform */
/* #undef ANDROID_HOST */

/* if the library is missing, don't use it. */
/* #undef BOOST_MULTI_INDEX_DISABLE_SERIALIZATION */

/* Use DirectFB device support */
/* #undef BUILD_DIRECTFB_DEVICE */

/* Build the EGL device for OpenVG, OpenGLES1&2, and X11/Mesa */
/* #undef BUILD_EGL_DEVICE */

/* Use raw Framebuffer device support */
/* #undef BUILD_RAWFB_DEVICE */

/* Add support for writing a standalone executable launcher for movies
   embedded in web pages */
/* #undef CREATE_STANDALONE_GNASH_LAUNCHER */

/* this is a Darwin platform */
/* #undef DARWIN_HOST */

/* Default Flash major version */
#define DEFAULT_FLASH_MAJOR_VERSION "10"

/* Default Flash minor version */
#define DEFAULT_FLASH_MINOR_VERSION "1"

/* Default 3-letter platform identifier for version string */
#define DEFAULT_FLASH_PLATFORM_ID "LNX"

/* Default Flash revision number */
#define DEFAULT_FLASH_REV_NUMBER "999"

/* Default value for System.capabilities.os */
#define DEFAULT_FLASH_SYSTEM_OS "GNU/Linux"

/* Path to default font */
//#define DEFAULT_FONTFILE "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
#define DEFAULT_FONTFILE "./freesans.ttf"

/* Default SharedObject base directory */
#define DEFAULT_SOL_SAFEDIR "~/.gnash/SharedObjects"

/* Default streams timeout in seconds */
#define DEFAULT_STREAMS_TIMEOUT 60

/* EGLNativeWindowType type */
/* #undef EGL_NATIVE_WINDOW_INT */

/* Disable double buffering for AGG */
/* #undef ENABLE_DOUBLE_BUFFERING */

/* Enable using a file instead of a real framebuffer */
/* #undef ENABLE_FAKE_FRAMEBUFFER */

/* Use FFmpeg for media decoding */
#if !defined(NOFFMPEG)
#define ENABLE_FFMPEG_MEDIA 1
#endif

/* Use gstreamer for media decoding */
/* #undef ENABLE_GST_MEDIA */

/* Use haiku for media decoding */
/* #undef ENABLE_HAIKU_MEDIA */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
/* #undef ENABLE_NLS */

/* this is a FreeBSD platform */
/* #undef FREEBSD_HOST */

/* Enable FPS debugging code */
/* #undef GNASH_FPS_DEBUG */

/* Collecting and report stats about ObjectURI case lookups */
/* #undef GNASH_STATS_OBJECT_URI_NOCASE */

/* Collecting and report stats about property lookups */
/* #undef GNASH_STATS_PROPERTY_LOOKUPS */

/* Collecting and report stats about string_table::key case lookups */
/* #undef GNASH_STATS_STRING_TABLE_NOCASE */

/* this is a GNU platform */
/* #undef GNU_HOST */

/* this is a Haiku platform */
/* #undef HAIKU_HOST */

/* Has the Gstreamer Plugin Dev package installed */
/* #undef HAS_GSTREAMER_PLUGINS_BASE */

/* Define this if you want to enable python usage */
/* #undef HAS_PYTHON */

/* Define if you have the atk/atk.h header */
/* #undef HAVE_ATK_ATK_H */

/* Define if avresample.h is found */
/* #undef HAVE_AVRESAMPLE_H */

/* Define if you have the cairo.h header */
/* #undef HAVE_CAIRO_H */

/* Define to 1 if you have the Mac OS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the Mac OS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Has clock_gettime() */
#define HAVE_CLOCK_GETTIME 1

/* Define to 1 if you have the <curl/curl.h> header file. */
/* #undef HAVE_CURL_CURL_H */

/* define if the compiler supports basic C++11 syntax */
/* #undef HAVE_CXX11 */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
/* #undef HAVE_DCGETTEXT */

/* Define if you have the dejagnu.h header */
/* #undef HAVE_DEJAGNU_H */

/* Define if you have the directfb.h header */
/* #undef HAVE_DIRECTFB_H */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Have OpenGL-ES EGL/egl.h */
/* #undef HAVE_EGL_EGL_H */

/* Define if you have the expat.h header */
#define HAVE_EXPAT_H 1

/* Has the Linux splice() system call */
#define HAVE_FCNTL_SPLICE 1

/* Has the Linux tee() system call */
#define HAVE_FCNTL_TEE 1

/* Define if you have avcodec.h installed. */
/* #undef HAVE_FFMPEG_AVCODEC_H */

/* Define if avformat.h is found */
/* #undef HAVE_FFMPEG_AVFORMAT_H */

/* Define if swresample.h is found */
/* #undef HAVE_FFMPEG_SWRESAMPLE_H */

/* Define if swscale.h is found */
/* #undef HAVE_FFMPEG_SWSCALE_H */

/* Defined if ffmpeg/vaapi.h is found */
/* #undef HAVE_FFMPEG_VAAPI_H */

/* Has finite */
#define HAVE_FINITE 1

/* Define if you have the fltk/FL_API.h header */
/* #undef HAVE_FLTK_FL_API_H */

/* Define if you have the fontconfig/fontconfig.h header */
/* #undef HAVE_FONTCONFIG_FONTCONFIG_H */

/* Define to 1 if you have the <freetype2/freetype/freetype.h> header file. */
/* #undef HAVE_FREETYPE2_FREETYPE_FREETYPE_H */

/* Define to 1 if you have the `ftime' function. */
#define HAVE_FTIME 1

/* __FUNCTION__ is defined */
#define HAVE_FUNCTION 1

/* Define to 1 if you have the `getopt' function. */
#define HAVE_GETOPT 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Has getpwnam */
#define HAVE_GETPWNAM 1

/* Define if the GNU gettext() function is already present or preinstalled. */
/* #undef HAVE_GETTEXT */

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the gif_lib.h header */
#if !defined(NOGIF)
#define HAVE_GIF_LIB_H 1
#endif

/* Have OpenGLES 1 egl.h */
/* #undef HAVE_GLES1_EGL_H */

/* Have OpenGL-ES GLES/glext.h */
/* #undef HAVE_GLES1_GLEXT_H */

/* Have OpenGL-ES GLES/gl.h */
/* #undef HAVE_GLES1_GL_H */

/* Define to 1 if you have the <GLES2/gl2.h> header file. */
/* #undef HAVE_GLES2_GL2_H */

/* Define to 1 if you have the <GLES/egl.h> header file. */
/* #undef HAVE_GLES_EGL_H */

/* Has GLIB library installed */
/* #undef HAVE_GLIB */

/* Define to 1 if you have the <GL/gl.h> header file. */
/* #undef HAVE_GL_GL_H */

/* Define this for GCC-visibility. */
/* #undef HAVE_GNUC_VISIBILITY */

/* Define if you have the gst/app/gstappsink.h header */
/* #undef HAVE_GST_APP_GSTAPPSINK_H */

/* Define if you have the gst/gst.h header */
/* #undef HAVE_GST_GST_H */

/* Define if you have the gst/interfaces/probeprobe.h header */
/* #undef HAVE_GST_INTERFACES_PROBEPROBE_H */

/* Define if you have the gst/pbutils/install-plugins.h header */
/* #undef HAVE_GST_PBUTILS_INSTALL_PLUGINS_H */

/* Use GTK2 for windowing */
/* #undef HAVE_GTK2 */

/* GTKGLExt header */
/* #undef HAVE_GTK_GTKGL_H */

/* Define to 1 if you have the <gtk/gtk.h> header file. */
/* #undef HAVE_GTK_GTK_H */

/* Define if you have the iconv() function and it works. */
/* #undef HAVE_ICONV */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Use ipc_info */
#define HAVE_IPC_INFO 1

/* Has isfinite */
#define HAVE_ISFINITE 1

/* Define if you have the jemalloc.h header */
/* #undef HAVE_JEMALLOC_H */

/* Define if you have the jpeglib.h header */
#define HAVE_JPEGLIB_H 1

/* Have KDE 3.x installed */
/* #undef HAVE_KDE3 */

/* Have KDE 4.x installed */
/* #undef HAVE_KDE4 */

/* Define to 1 if you have the <kde4/kapplication.h> header file. */
/* #undef HAVE_KDE4_KAPPLICATION_H */

/* Define to 1 if you have the <kde/qxembed.h> header file. */
/* #undef HAVE_KDE_QXEMBED_H */

/* Define if you have avcodec.h installed. */
#define HAVE_LIBAVCODEC_AVCODEC_H 1

/* Defined if libavcodec/vaapi.h is found */
#define HAVE_LIBAVCODEC_VAAPI_H 1

/* Define if avformat.h is found */
#define HAVE_LIBAVFORMAT_AVFORMAT_H 1

/* Define if avresample.h is found */
/* #undef #define HAVE_LIBAVRESAMPLE_AVRESAMPLE_H */

/* Define if libavutil/opt.h is found */
#define HAVE_LIBAVUTIL_OPT_H 1

/* Define if avresample.h is found */
/* #undef HAVE_LIBAV_AVRESAMPLE_H */

/* Define to 1 if you have the <libgen.h> header file. */
#define HAVE_LIBGEN_H 1

/* Define to 1 if you have the `rt' library (-lrt). */
#define HAVE_LIBRT 1

/* Define if you have the libssh/libssh.h header */
/* #undef HAVE_LIBSSH_LIBSSH_H */

/* Define if swresample.h is found */
#define HAVE_LIBSWRESAMPLE_SWRESAMPLE_H 1

/* Define if swscale.h is found */
#define HAVE_LIBSWSCALE_SWSCALE_H 1

/* Define to 1 if you have the <linux/uinput.h> header file. */
#define HAVE_LINUX_UINPUT_H 1

/* Define to 1 if you have the `localtime_r' function. */
#define HAVE_LOCALTIME_R 1

/* extern timezone is a long integer, not a function */
#define HAVE_LONG_TIMEZONE 1

/* Define if you have the ltdl.h header */
#define HAVE_LTDL_H 1

/* Has mallinfo() */
/* #undef HAVE_MALLINFO */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <ming.h> header file. */
/* #undef HAVE_MING_H */

/* Define to 1 if you have the `mkstemps' function. */
#define HAVE_MKSTEMPS 1

/* Define to 1 if you have the `mmap' function. */
#define HAVE_MMAP 1

/* Defined if you have MySQL installed */
/* #undef HAVE_MYSQL */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define if you have the nspr.h header */
/* #undef HAVE_NSPR_H */

/* Define if you have the openssl/ssl.h header */
/* #undef HAVE_OPENSSL_SSL_H */

/* Define if you have the png.h header */
#if !defined(NOPNG)
#define HAVE_PNG_H 1
#endif

/* Define to 1 if you have the <poll.h> header file. */
#define HAVE_POLL_H 1

/* Define to 1 if you have the `ppoll' function. */
#define HAVE_PPOLL 1

/* __PRETTY_FUNCTION__ is defined */
#define HAVE_PRETTY_FUNCTION 1

/* Define to 1 if you have the `pselect' function. */
#define HAVE_PSELECT 1

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREADS 1

/* Define to 1 if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define to 1 if you have the <pythonrun.h> header file. */
/* #undef HAVE_PYTHONRUN_H */

/* Have QT 3.x installed */
/* #undef HAVE_QT3 */

/* Have QT 4.x installed */
/* #undef HAVE_QT4 */

/* Define to 1 if you have the `scandir' function. */
#define HAVE_SCANDIR 1

/* We have SDL support */
#define HAVE_SDL_H 1

/* Define to 1 if you have the `semget' function. */
#define HAVE_SEMGET 1

/* Has the Linux sendfile() system call */
#define HAVE_SENDFILE 1

/* Define to 1 if you have the `shmat' function. */
#define HAVE_SHMAT 1

/* Define to 1 if you have the `shmdt' function. */
#define HAVE_SHMDT 1

/* Define to 1 if you have the `shmget' function. */
#define HAVE_SHMGET 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define if you have the speex/speex.h header */
/* #undef HAVE_SPEEX_SPEEX_H */

/* Define if you have the speex/speex_resampler.h header */
/* #undef HAVE_SPEEX_SPEEX_RESAMPLER_H */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if swresample.h is found */
/* #undef HAVE_SWRESAMPLE_H */

/* Define if swscale.h is found */
/* #undef HAVE_SWSCALE_H */

/* Define to 1 if you have the `sysconf' function. */
#define HAVE_SYSCONF 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/epoll.h> header file. */
#define HAVE_SYS_EPOLL_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/utsname.h> header file. */
#define HAVE_SYS_UTSNAME_H 1

/* struct tm has member tm_gmtoff */
#define HAVE_TM_GMTOFF 1

/* Define if you have the tslib.h header */
/* #undef HAVE_TSLIB_H */

/* Define to 1 if you have the `tzset' function. */
#define HAVE_TZSET 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have the va/va_glx.h header */
/* #undef HAVE_VA_VA_GLX_H */

/* Define if you have the va/va.h header */
/* #undef HAVE_VA_VA_H */

/* Define if you have the va/va_x11.h header */
/* #undef HAVE_VA_VA_X11_H */

/* Have Freescale OpenVG */
/* #undef HAVE_VG_EXT_H */

/* Have LibMESA OpenVG */
/* #undef HAVE_VG_VGEXT_H */

/* This is defined is we are on Win32 */
/* #undef HAVE_WINSOCK */

/* Define to 1 if you have the <winsock2.h> header file. */
/* #undef HAVE_WINSOCK2_H */

/* Define to 1 if you have the <winsock.h> header file. */
/* #undef HAVE_WINSOCK_H */

/* X11 headers and libraries */
#define HAVE_X11 1

/* Have the X11 X.h header file */
#define HAVE_X11_X_H 1

/* Define if you have the Xft.h header */
/* #undef HAVE_XFT_H */

/* Define if you have the zlib.h header */
#define HAVE_ZLIB_H 1

/* __func__ is defined */
#define HAVE_func 1

/* Has the key field in ipc_perm */
#define IPC_PERM_KEY key

/* this is a Linux platform */
#define LINUX_HOST 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* this is a NetBSD platform */
/* #undef NETBSD_HOST */

/* Is not based on the SGI GL */
/* #undef NOT_SGI_GL */

/* Newer versions use const in prototypes */
/* #undef NPAPI_CONST */

/* Define that we have NPAPI present in version 1.9.0 and maybe older */
/* #undef NPAPI_VERSION */

/* this is an OpenBSD platform */
/* #undef OPENBSD_HOST */

/* Name of package */
#define PACKAGE "gnash"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "gnash"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "gnash 0.8.11dev"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "gnash"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.8.11dev"

/* ABGR32 pixel format */
/* #undef PIXELFORMAT_ABGR32 */

/* ARGB32 pixel format */
/* #undef PIXELFORMAT_ARGB32 */

/* BGR24 pixel format */
/* #undef PIXELFORMAT_BGR24 */

/* BGRA32 pixel format */
/* #undef PIXELFORMAT_BGRA32 */

/* RGB24 pixel format */
/* #undef PIXELFORMAT_RGB24 */

/* RGB555 pixel format */
/* #undef PIXELFORMAT_RGB555 */

/* RGB565 pixel format */
/* #undef PIXELFORMAT_RGB565 */

/* RGBA32 pixel format */
/* #undef PIXELFORMAT_RGBA32 */

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Use AntiGrain renderer */
/* #undef RENDERER_AGG */

/* Use cairo renderer */
/* #undef RENDERER_CAIRO */

/* Use OpenGL-ES version 1.x */
/* #undef RENDERER_GLES1 */

/* Use OpenGLES v2 version 2 */
/* #undef RENDERER_GLES2 */

/* Use OpenGL renderer */
/* #undef RENDERER_OPENGL */

/* Use OpenVG renderer */
/* #undef RENDERER_OPENVG */

/* this is a Solaris platform */
/* #undef SOLARIS_HOST */

/* Use AmigaOS AHI for sound handling */
/* #undef SOUND_AHI */

/* Use Haiku Media Kit for sound handling */
/* #undef SOUND_MKIT */

/* Use SDL for sound handling */
#define SOUND_SDL 1

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Build the DBUS extension */
/* #undef USE_DBUS_EXT */

/* Build the DejaGnu extension */
/* #undef USE_DEJAGNU_EXT */

/* Specify that extension support is enabled. */
/* #undef USE_EXTENSIONS */

/* Build the FileIO extension */
/* #undef USE_FILEIO_EXT */

/* Define this if you want to enable freetype usage */
#if !defined(NOFREETYPE)
#define USE_FREETYPE 1
#endif

/* Use the GIF library */
#if !defined(NOGIF)
#define USE_GIF 1
#endif

/* Use GtkGLExt extension */
/* #undef USE_GTKGLEXT */

/* Build all the extensions */
/* #undef USE_GTK_EXT */

/* Add support for a directly using Linux Input Event Devices */
/* #undef USE_INPUT_EVENTS */

/* Add support for directly using a PS/2 Keyboard */
/* #undef USE_KEYBOARD_PS2 */

/* Support LocalConnection */
#define USE_LC 1

/* Build the LIRC extension */
/* #undef USE_LIRC_EXT */

/* Build any media code */
#if !defined(NOMEDIA)
#define USE_MEDIA 1
#endif

/* GUI Menu support */
/* #undef USE_MENUS */

/* Add support for a directly using a PS/2 Mouse */
/* #undef USE_MOUSE_PS2 */

/* Build the MySQL extension */
/* #undef USE_MYSQL_EXT */

/* Use the PNG library */
#if !defined(NOPNG)
#define USE_PNG 1
#endif

/* Shared Objects are read-only */
/* #undef USE_SOL_READONLY */

/* Build any sound code */
#if !defined(NOSOUND)
#define USE_SOUND 1
#endif

/* Use SSH for authentication */
/* #undef USE_SSH */

/* Use SSL for authentication */
/* #undef USE_SSL */

/* View SWF information */
/* #undef USE_SWFTREE */

/* Testsuite support, maintainers option only */
/* #undef USE_TESTSUITE */

/* Use a tslib supported touchscreen */
/* #undef USE_TSLIB */

/* Version number of package */
#define VERSION "0.8.11dev"

/* this is a Win32 platform */
/* #undef WIN32_HOST */

/* this is a WINCE6 platform */
/* #undef WINCE6_HOST */

/* this is a WINCE platform */
/* #undef WINCE_HOST */

/* Define if GConf support is enabled */
/* #undef WITH_GCONF */

/* this is a 64 platform */
/* #undef WORDSIZE */

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Write files while streaming */
/* #undef WRITE_FILE */

/* this is a 64 bit powerpc */
/* #undef __powerpc64__ */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif
