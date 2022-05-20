Gnash OpenDingux/GKD/Funkey/Bittboy port
=========================================

This is a trimmed down port of Gnash for OpenDingux compatible devices.
Some ports will need a boost folder relative to the Makefile.
However, it only needs the headers.

(Previously you would need to compile boost program options but that dependency has been removed)

Here are the list of dependencies required for this port of Gnash :
- libc
- an STL C++ library
- SDL 1.2 (Although the codebase supports fbdev, i would recommend you avoid that if possible)
