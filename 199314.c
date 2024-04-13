
    void _map_window() {
      Display *const dpy = cimg::X11_attr().display;
      bool is_exposed = false, is_mapped = false;
      XWindowAttributes attr;
      XEvent event;
      XMapRaised(dpy,_window);
      do { // Wait for the window to be mapped.
        XWindowEvent(dpy,_window,StructureNotifyMask | ExposureMask,&event);
        switch (event.type) {
        case MapNotify : is_mapped = true; break;
        case Expose : is_exposed = true; break;
        }
      } while (!is_exposed || !is_mapped);
      do { // Wait for the window to be visible.
        XGetWindowAttributes(dpy,_window,&attr);
        if (attr.map_state!=IsViewable) { XSync(dpy,0); cimg::sleep(10); }
      } while (attr.map_state!=IsViewable);
      _window_x = attr.x;
      _window_y = attr.y;