
    void _paint(const bool wait_expose=true) {
      if (_is_closed || !_image) return;
      Display *const dpy = cimg::X11_attr().display;
      if (wait_expose) { // Send an expose event sticked to display window to force repaint.
        XEvent event;
        event.xexpose.type = Expose;
        event.xexpose.serial = 0;
        event.xexpose.send_event = 1;
        event.xexpose.display = dpy;
        event.xexpose.window = _window;
        event.xexpose.x = 0;
        event.xexpose.y = 0;
        event.xexpose.width = width();
        event.xexpose.height = height();
        event.xexpose.count = 0;
        XSendEvent(dpy,_window,0,0,&event);
      } else { // Repaint directly (may be called from the expose event).
        GC gc = DefaultGC(dpy,DefaultScreen(dpy));
#ifdef cimg_use_xshm
        if (_shminfo) XShmPutImage(dpy,_window,gc,_image,0,0,0,0,_width,_height,1);
        else XPutImage(dpy,_window,gc,_image,0,0,0,0,_width,_height);
#else
        XPutImage(dpy,_window,gc,_image,0,0,0,0,_width,_height);
#endif
      }