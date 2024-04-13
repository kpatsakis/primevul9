
    CImgDisplay& assign() {
      if (is_empty()) return flush();
      Display *const dpy = cimg::X11_attr().display;
      cimg_lock_display();

      // Remove display window from event thread list.
      unsigned int i;
      for (i = 0; i<cimg::X11_attr().nb_wins && cimg::X11_attr().wins[i]!=this; ++i) {}
      for ( ; i<cimg::X11_attr().nb_wins - 1; ++i) cimg::X11_attr().wins[i] = cimg::X11_attr().wins[i + 1];
      --cimg::X11_attr().nb_wins;

      // Destroy window, image, colormap and title.
      if (_is_fullscreen && !_is_closed) _desinit_fullscreen();
      XDestroyWindow(dpy,_window);
      _window = 0;
#ifdef cimg_use_xshm
      if (_shminfo) {
        XShmDetach(dpy,_shminfo);
        XDestroyImage(_image);
        shmdt(_shminfo->shmaddr);
        shmctl(_shminfo->shmid,IPC_RMID,0);
        delete _shminfo;
        _shminfo = 0;
      } else
#endif
        XDestroyImage(_image);
      _data = 0; _image = 0;
      if (cimg::X11_attr().nb_bits==8) XFreeColormap(dpy,_colormap);
      _colormap = 0;
      XSync(dpy,0);

      // Reset display variables.
      delete[] _title;
      _width = _height = _normalization = _window_width = _window_height = 0;
      _window_x = _window_y = 0;
      _is_fullscreen = false;
      _is_closed = true;
      _min = _max = 0;
      _title = 0;
      flush();

      cimg_unlock_display();
      return *this;