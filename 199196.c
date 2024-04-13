
    void _desinit_fullscreen() {
      if (!_is_fullscreen) return;
      Display *const dpy = cimg::X11_attr().display;
      XUngrabKeyboard(dpy,CurrentTime);
#ifdef cimg_use_xrandr
      if (cimg::X11_attr().resolutions && cimg::X11_attr().curr_resolution) {
        XRRScreenConfiguration *config = XRRGetScreenInfo(dpy,DefaultRootWindow(dpy));
        XRRSetScreenConfig(dpy,config,DefaultRootWindow(dpy),0,cimg::X11_attr().curr_rotation,CurrentTime);
        XRRFreeScreenConfigInfo(config);
        XSync(dpy,0);
        cimg::X11_attr().curr_resolution = 0;
      }
#endif
      if (_background_window) XDestroyWindow(dpy,_background_window);
      _background_window = 0;
      _is_fullscreen = false;