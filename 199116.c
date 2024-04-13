
    CImgDisplay& resize(const int nwidth, const int nheight, const bool force_redraw=true) {
      if (!nwidth || !nheight || (is_empty() && (nwidth<0 || nheight<0))) return assign();
      if (is_empty()) return assign(nwidth,nheight);
      Display *const dpy = cimg::X11_attr().display;
      const unsigned int
        tmpdimx = (nwidth>0)?nwidth:(-nwidth*width()/100),
        tmpdimy = (nheight>0)?nheight:(-nheight*height()/100),
        dimx = tmpdimx?tmpdimx:1,
        dimy = tmpdimy?tmpdimy:1;
      if (_width!=dimx || _height!=dimy || _window_width!=dimx || _window_height!=dimy) {
        show();
        cimg_lock_display();
        if (_window_width!=dimx || _window_height!=dimy) {
          XWindowAttributes attr;
          for (unsigned int i = 0; i<10; ++i) {
            XResizeWindow(dpy,_window,dimx,dimy);
            XGetWindowAttributes(dpy,_window,&attr);
            if (attr.width==(int)dimx && attr.height==(int)dimy) break;
            cimg::wait(5);
          }
        }
        if (_width!=dimx || _height!=dimy) switch (cimg::X11_attr().nb_bits) {
          case 8 :  { unsigned char pixel_type = 0; _resize(pixel_type,dimx,dimy,force_redraw); } break;
          case 16 : { unsigned short pixel_type = 0; _resize(pixel_type,dimx,dimy,force_redraw); } break;
          default : { unsigned int pixel_type = 0; _resize(pixel_type,dimx,dimy,force_redraw); }
          }
        _window_width = _width = dimx; _window_height = _height = dimy;
        cimg_unlock_display();
      }
      _is_resized = false;
      if (_is_fullscreen) move((screen_width() - _width)/2,(screen_height() - _height)/2);
      if (force_redraw) return paint();
      return *this;