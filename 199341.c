
    void _init_fullscreen() {
      if (!_is_fullscreen || _is_closed) return;
      Display *const dpy = cimg::X11_attr().display;
      _background_window = 0;

#ifdef cimg_use_xrandr
      int foo;
      if (XRRQueryExtension(dpy,&foo,&foo)) {
        XRRRotations(dpy,DefaultScreen(dpy),&cimg::X11_attr().curr_rotation);
        if (!cimg::X11_attr().resolutions) {
          cimg::X11_attr().resolutions = XRRSizes(dpy,DefaultScreen(dpy),&foo);
          cimg::X11_attr().nb_resolutions = (unsigned int)foo;
        }
        if (cimg::X11_attr().resolutions) {
          cimg::X11_attr().curr_resolution = 0;
          for (unsigned int i = 0; i<cimg::X11_attr().nb_resolutions; ++i) {
            const unsigned int
              nw = (unsigned int)(cimg::X11_attr().resolutions[i].width),
              nh = (unsigned int)(cimg::X11_attr().resolutions[i].height);
            if (nw>=_width && nh>=_height &&
                nw<=(unsigned int)(cimg::X11_attr().resolutions[cimg::X11_attr().curr_resolution].width) &&
                nh<=(unsigned int)(cimg::X11_attr().resolutions[cimg::X11_attr().curr_resolution].height))
              cimg::X11_attr().curr_resolution = i;
          }
          if (cimg::X11_attr().curr_resolution>0) {
            XRRScreenConfiguration *config = XRRGetScreenInfo(dpy,DefaultRootWindow(dpy));
            XRRSetScreenConfig(dpy,config,DefaultRootWindow(dpy),
                               cimg::X11_attr().curr_resolution,cimg::X11_attr().curr_rotation,CurrentTime);
            XRRFreeScreenConfigInfo(config);
            XSync(dpy,0);
          }
        }
      }
      if (!cimg::X11_attr().resolutions)
        cimg::warn(_cimgdisplay_instance
                   "init_fullscreen(): Xrandr extension not supported by the X server.",
                   cimgdisplay_instance);
#endif

      const unsigned int sx = screen_width(), sy = screen_height();
      if (sx==_width && sy==_height) return;
      XSetWindowAttributes winattr;
      winattr.override_redirect = 1;
      _background_window = XCreateWindow(dpy,DefaultRootWindow(dpy),0,0,sx,sy,0,0,
                                         InputOutput,CopyFromParent,CWOverrideRedirect,&winattr);
      const cimg_ulong buf_size = (cimg_ulong)sx*sy*(cimg::X11_attr().nb_bits==8?1:
                                                     (cimg::X11_attr().nb_bits==16?2:4));
      void *background_data = std::malloc(buf_size);
      std::memset(background_data,0,buf_size);
      XImage *background_image = XCreateImage(dpy,DefaultVisual(dpy,DefaultScreen(dpy)),cimg::X11_attr().nb_bits,
                                              ZPixmap,0,(char*)background_data,sx,sy,8,0);
      XEvent event;
      XSelectInput(dpy,_background_window,StructureNotifyMask);
      XMapRaised(dpy,_background_window);
      do XWindowEvent(dpy,_background_window,StructureNotifyMask,&event);
      while (event.type!=MapNotify);
      GC gc = DefaultGC(dpy,DefaultScreen(dpy));
#ifdef cimg_use_xshm
      if (_shminfo) XShmPutImage(dpy,_background_window,gc,background_image,0,0,0,0,sx,sy,0);
      else XPutImage(dpy,_background_window,gc,background_image,0,0,0,0,sx,sy);
#else
      XPutImage(dpy,_background_window,gc,background_image,0,0,0,0,sx,sy);
#endif
      XWindowAttributes attr;
      XGetWindowAttributes(dpy,_background_window,&attr);
      while (attr.map_state!=IsViewable) XSync(dpy,0);
      XDestroyImage(background_image);