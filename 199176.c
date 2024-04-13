
    void _handle_events(const XEvent *const pevent) {
      Display *const dpy = cimg::X11_attr().display;
      XEvent event = *pevent;
      switch (event.type) {
      case ClientMessage : {
        if ((int)event.xclient.message_type==(int)_wm_protocol_atom &&
            (int)event.xclient.data.l[0]==(int)_wm_window_atom) {
          XUnmapWindow(cimg::X11_attr().display,_window);
          _is_closed = _is_event = true;
          pthread_cond_broadcast(&cimg::X11_attr().wait_event);
        }
      } break;
      case ConfigureNotify : {
        while (XCheckWindowEvent(dpy,_window,StructureNotifyMask,&event)) {}
        const unsigned int nw = event.xconfigure.width, nh = event.xconfigure.height;
        const int nx = event.xconfigure.x, ny = event.xconfigure.y;
        if (nw && nh && (nw!=_window_width || nh!=_window_height)) {
          _window_width = nw; _window_height = nh; _mouse_x = _mouse_y = -1;
          XResizeWindow(dpy,_window,_window_width,_window_height);
          _is_resized = _is_event = true;
          pthread_cond_broadcast(&cimg::X11_attr().wait_event);
        }
        if (nx!=_window_x || ny!=_window_y) {
          _window_x = nx; _window_y = ny; _is_moved = _is_event = true;
          pthread_cond_broadcast(&cimg::X11_attr().wait_event);
        }
      } break;
      case Expose : {
        while (XCheckWindowEvent(dpy,_window,ExposureMask,&event)) {}
        _paint(false);
        if (_is_fullscreen) {
          XWindowAttributes attr;
          XGetWindowAttributes(dpy,_window,&attr);
          while (attr.map_state!=IsViewable) XSync(dpy,0);
          XSetInputFocus(dpy,_window,RevertToParent,CurrentTime);
        }
      } break;
      case ButtonPress : {
        do {
          _mouse_x = event.xmotion.x; _mouse_y = event.xmotion.y;
          if (_mouse_x<0 || _mouse_y<0 || _mouse_x>=width() || _mouse_y>=height()) _mouse_x = _mouse_y = -1;
          switch (event.xbutton.button) {
          case 1 : set_button(1); break;
          case 3 : set_button(2); break;
          case 2 : set_button(3); break;
          }
        } while (XCheckWindowEvent(dpy,_window,ButtonPressMask,&event));
      } break;
      case ButtonRelease : {
        do {
          _mouse_x = event.xmotion.x; _mouse_y = event.xmotion.y;
          if (_mouse_x<0 || _mouse_y<0 || _mouse_x>=width() || _mouse_y>=height()) _mouse_x = _mouse_y = -1;
          switch (event.xbutton.button) {
          case 1 : set_button(1,false); break;
          case 3 : set_button(2,false); break;
          case 2 : set_button(3,false); break;
          case 4 : set_wheel(1); break;
          case 5 : set_wheel(-1); break;
          }
        } while (XCheckWindowEvent(dpy,_window,ButtonReleaseMask,&event));
      } break;
      case KeyPress : {
        char tmp = 0; KeySym ksym;
        XLookupString(&event.xkey,&tmp,1,&ksym,0);
        set_key((unsigned int)ksym,true);
      } break;
      case KeyRelease : {
        char keys_return[32];  // Check that the key has been physically unpressed.
        XQueryKeymap(dpy,keys_return);
        const unsigned int kc = event.xkey.keycode, kc1 = kc/8, kc2 = kc%8;
        const bool is_key_pressed = kc1>=32?false:(keys_return[kc1]>>kc2)&1;
        if (!is_key_pressed) {
          char tmp = 0; KeySym ksym;
          XLookupString(&event.xkey,&tmp,1,&ksym,0);
          set_key((unsigned int)ksym,false);
        }
      } break;
      case EnterNotify: {
        while (XCheckWindowEvent(dpy,_window,EnterWindowMask,&event)) {}
        _mouse_x = event.xmotion.x;
        _mouse_y = event.xmotion.y;
        if (_mouse_x<0 || _mouse_y<0 || _mouse_x>=width() || _mouse_y>=height()) _mouse_x = _mouse_y = -1;
      } break;
      case LeaveNotify : {
        while (XCheckWindowEvent(dpy,_window,LeaveWindowMask,&event)) {}
        _mouse_x = _mouse_y = -1; _is_event = true;
        pthread_cond_broadcast(&cimg::X11_attr().wait_event);
      } break;
      case MotionNotify : {
        while (XCheckWindowEvent(dpy,_window,PointerMotionMask,&event)) {}
        _mouse_x = event.xmotion.x;
        _mouse_y = event.xmotion.y;
        if (_mouse_x<0 || _mouse_y<0 || _mouse_x>=width() || _mouse_y>=height()) _mouse_x = _mouse_y = -1;
        _is_event = true;
        pthread_cond_broadcast(&cimg::X11_attr().wait_event);
      } break;
      }