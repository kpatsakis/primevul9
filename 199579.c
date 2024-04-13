
    static void* _events_thread(void *arg) { // Thread to manage events for all opened display windows.
      Display *const dpy = cimg::X11_attr().display;
      XEvent event;
      pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,0);
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
      if (!arg) for ( ; ; ) {
        cimg_lock_display();
        bool event_flag = XCheckTypedEvent(dpy,ClientMessage,&event);
        if (!event_flag) event_flag = XCheckMaskEvent(dpy,
                                                      ExposureMask | StructureNotifyMask | ButtonPressMask |
                                                      KeyPressMask | PointerMotionMask | EnterWindowMask |
                                                      LeaveWindowMask | ButtonReleaseMask | KeyReleaseMask,&event);
        if (event_flag)
          for (unsigned int i = 0; i<cimg::X11_attr().nb_wins; ++i)
            if (!cimg::X11_attr().wins[i]->_is_closed && event.xany.window==cimg::X11_attr().wins[i]->_window)
              cimg::X11_attr().wins[i]->_handle_events(&event);
        cimg_unlock_display();
        pthread_testcancel();
        cimg::sleep(8);
      }
      return 0;