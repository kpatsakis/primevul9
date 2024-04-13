
    static int screen_width() {
      Display *const dpy = cimg::X11_attr().display;
      int res = 0;
      if (!dpy) {
        Display *const _dpy = XOpenDisplay(0);
        if (!_dpy)
          throw CImgDisplayException("CImgDisplay::screen_width(): Failed to open X11 display.");
        res = DisplayWidth(_dpy,DefaultScreen(_dpy));
        XCloseDisplay(_dpy);
      } else {
#ifdef cimg_use_xrandr
        if (cimg::X11_attr().resolutions && cimg::X11_attr().curr_resolution)
          res = cimg::X11_attr().resolutions[cimg::X11_attr().curr_resolution].width;
        else res = DisplayWidth(dpy,DefaultScreen(dpy));
#else
        res = DisplayWidth(dpy,DefaultScreen(dpy));
#endif
      }
      return res;