
    const CImg<T>& _display(CImgDisplay &disp, const char *const title, const bool display_info,
                            unsigned int *const XYZ, const bool exit_on_anykey,
                            const bool exit_on_simpleclick) const {
      unsigned int oldw = 0, oldh = 0, _XYZ[3] = { 0 }, key = 0;
      int x0 = 0, y0 = 0, z0 = 0, x1 = width() - 1, y1 = height() - 1, z1 = depth() - 1,
        old_mouse_x = -1, old_mouse_y = -1;

      if (!disp) {
        disp.assign(cimg_fitscreen(_width,_height,_depth),title?title:0,1);
        if (!title) disp.set_title("CImg<%s> (%ux%ux%ux%u)",pixel_type(),_width,_height,_depth,_spectrum);
        else disp.set_title("%s",title);
      } else if (title) disp.set_title("%s",title);
      disp.show().flush();

      const CImg<char> dtitle = CImg<char>::string(disp.title());
      if (display_info) print(dtitle);

      CImg<T> zoom;
      for (bool reset_view = true, resize_disp = false, is_first_select = true; !key && !disp.is_closed(); ) {
        if (reset_view) {
          if (XYZ) { _XYZ[0] = XYZ[0]; _XYZ[1] = XYZ[1]; _XYZ[2] = XYZ[2]; }
          else {
            _XYZ[0] = (unsigned int)(x0 + x1)/2;
            _XYZ[1] = (unsigned int)(y0 + y1)/2;
            _XYZ[2] = (unsigned int)(z0 + z1)/2;
          }
          x0 = 0; y0 = 0; z0 = 0; x1 = width() - 1; y1 = height() - 1; z1 = depth() - 1;
          oldw = disp._width; oldh = disp._height;
          reset_view = false;
        }
        if (!x0 && !y0 && !z0 && x1==width() - 1 && y1==height() - 1 && z1==depth() - 1) {
          if (is_empty()) zoom.assign(1,1,1,1,(T)0); else zoom.assign();
        } else zoom = get_crop(x0,y0,z0,x1,y1,z1);

        const CImg<T>& visu = zoom?zoom:*this;
        const unsigned int
          dx = 1U + x1 - x0, dy = 1U + y1 - y0, dz = 1U + z1 - z0,
          tw = dx + (dz>1?dz:0U), th = dy + (dz>1?dz:0U);
        if (!is_empty() && !disp.is_fullscreen() && resize_disp) {
          const unsigned int
            ttw = tw*disp.width()/oldw, tth = th*disp.height()/oldh,
            dM = std::max(ttw,tth), diM = (unsigned int)std::max(disp.width(),disp.height()),
            imgw = std::max(16U,ttw*diM/dM), imgh = std::max(16U,tth*diM/dM);
          disp.set_fullscreen(false).resize(cimg_fitscreen(imgw,imgh,1),false);
          resize_disp = false;
        }
        oldw = tw; oldh = th;

        bool
          go_up = false, go_down = false, go_left = false, go_right = false,
          go_inc = false, go_dec = false, go_in = false, go_out = false,
          go_in_center = false;

        disp.set_title("%s",dtitle._data);
        if (_width>1 && visu._width==1) disp.set_title("%s | x=%u",disp._title,x0);
        if (_height>1 && visu._height==1) disp.set_title("%s | y=%u",disp._title,y0);
        if (_depth>1 && visu._depth==1) disp.set_title("%s | z=%u",disp._title,z0);

        disp._mouse_x = old_mouse_x; disp._mouse_y = old_mouse_y;
        CImg<intT> selection = visu._select(disp,0,2,_XYZ,x0,y0,z0,true,is_first_select,_depth>1);
        old_mouse_x = disp._mouse_x; old_mouse_y = disp._mouse_y;
        is_first_select = false;

        if (disp.wheel()) {
          if ((disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) &&
              (disp.is_keySHIFTLEFT() || disp.is_keySHIFTRIGHT())) {
            go_left = !(go_right = disp.wheel()>0);
          } else if (disp.is_keySHIFTLEFT() || disp.is_keySHIFTRIGHT()) {
            go_down = !(go_up = disp.wheel()>0);
          } else if (depth()==1 || disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            go_out = !(go_in = disp.wheel()>0); go_in_center = false;
          }
          disp.set_wheel();
        }

        if (disp.is_keyCTRLLEFT()) { // Alternative way for zooming and selection.
          if (selection[2]==selection[5]) { selection[2] = 0; selection[5] = visu.depth() - 1; }
          else if (selection[1]==selection[4]) { selection[1] = 0; selection[4] = visu.height() - 1; }
          else if (selection[0]==selection[3]) { selection[0] = 0; selection[3] = visu.width() - 1; }
        }

        const int
          sx0 = selection(0), sy0 = selection(1), sz0 = selection(2),
          sx1 = selection(3), sy1 = selection(4), sz1 = selection(5);
        if (sx0>=0 && sy0>=0 && sz0>=0 && sx1>=0 && sy1>=0 && sz1>=0) {
          x1 = x0 + sx1; y1 = y0 + sy1; z1 = z0 + sz1;
          x0+=sx0; y0+=sy0; z0+=sz0;
          if (sx0==sx1 && sy0==sy1 && sz0==sz1) {
            if (exit_on_simpleclick && (!zoom || is_empty())) break; else reset_view = true;
          }
          resize_disp = true;
        } else switch (key = disp.key()) {
#if cimg_OS!=2
          case cimg::keyCTRLRIGHT : case cimg::keySHIFTRIGHT :
#endif
          case 0 : case cimg::keyCTRLLEFT : case cimg::keySHIFTLEFT : key = 0; break;
          case cimg::keyP : if (visu._depth>1 && (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT())) {
              // Special mode: play stack of frames
              const unsigned int
                w1 = visu._width*disp.width()/(visu._width + (visu._depth>1?visu._depth:0)),
                h1 = visu._height*disp.height()/(visu._height + (visu._depth>1?visu._depth:0));
              float frame_timing = 5;
              bool is_stopped = false;
              disp.set_key(key,false).set_wheel().resize(cimg_fitscreen(w1,h1,1),false); key = 0;
              for (unsigned int timer = 0; !key && !disp.is_closed() && !disp.button(); ) {
                if (disp.is_resized()) disp.resize(false);
                if (!timer) {
                  visu.get_slice((int)_XYZ[2]).display(disp.set_title("%s | z=%d",dtitle.data(),_XYZ[2]));
                  (++_XYZ[2])%=visu._depth;
                }
                if (!is_stopped) { if (++timer>(unsigned int)frame_timing) timer = 0; } else timer = ~0U;
                if (disp.wheel()) { frame_timing-=disp.wheel()/3.0f; disp.set_wheel(); }
                switch (key = disp.key()) {
#if cimg_OS!=2
                case cimg::keyCTRLRIGHT :
#endif
                case cimg::keyCTRLLEFT : key = 0; break;
                case cimg::keyPAGEUP : frame_timing-=0.3f; key = 0; break;
                case cimg::keyPAGEDOWN : frame_timing+=0.3f; key = 0; break;
                case cimg::keySPACE : is_stopped = !is_stopped; disp.set_key(key,false); key = 0; break;
                case cimg::keyARROWLEFT : case cimg::keyARROWUP : is_stopped = true; timer = 0; key = 0; break;
                case cimg::keyARROWRIGHT : case cimg::keyARROWDOWN : is_stopped = true;
                  (_XYZ[2]+=visu._depth - 2)%=visu._depth; timer = 0; key = 0; break;
                case cimg::keyD : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
                    disp.set_fullscreen(false).
                      resize(CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,false),
                             CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,true),false);
                    disp.set_key(key,false); key = 0;
                  } break;
                case cimg::keyC : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
                    disp.set_fullscreen(false).
                      resize(cimg_fitscreen(2*disp.width()/3,2*disp.height()/3,1),false).set_key(key,false); key = 0;
                  } break;
                case cimg::keyR : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
                    disp.set_fullscreen(false).
                      resize(cimg_fitscreen(_width,_height,_depth),false).set_key(key,false); key = 0;
                  } break;
                case cimg::keyF : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
                    disp.resize(disp.screen_width(),disp.screen_height(),false).
                      toggle_fullscreen().set_key(key,false); key = 0;
                  } break;
                }
                frame_timing = frame_timing<1?1:(frame_timing>39?39:frame_timing);
                disp.wait(20);
              }
              const unsigned int
                w2 = (visu._width + (visu._depth>1?visu._depth:0))*disp.width()/visu._width,
                h2 = (visu._height + (visu._depth>1?visu._depth:0))*disp.height()/visu._height;
              disp.resize(cimg_fitscreen(w2,h2,1),false).set_title(dtitle.data()).set_key().set_button().set_wheel();
              key = 0;
            } break;
          case cimg::keyHOME : reset_view = resize_disp = true; key = 0; break;
          case cimg::keyPADADD : go_in = true; go_in_center = true; key = 0; break;
          case cimg::keyPADSUB : go_out = true; key = 0; break;
          case cimg::keyARROWLEFT : case cimg::keyPAD4: go_left = true; key = 0; break;
          case cimg::keyARROWRIGHT : case cimg::keyPAD6: go_right = true; key = 0; break;
          case cimg::keyARROWUP : case cimg::keyPAD8: go_up = true; key = 0; break;
          case cimg::keyARROWDOWN : case cimg::keyPAD2: go_down = true; key = 0; break;
          case cimg::keyPAD7 : go_up = go_left = true; key = 0; break;
          case cimg::keyPAD9 : go_up = go_right = true; key = 0; break;
          case cimg::keyPAD1 : go_down = go_left = true; key = 0; break;
          case cimg::keyPAD3 : go_down = go_right = true; key = 0; break;
          case cimg::keyPAGEUP : go_inc = true; key = 0; break;
          case cimg::keyPAGEDOWN : go_dec = true; key = 0; break;
          }
        if (go_in) {
          const int
            mx = go_in_center?disp.width()/2:disp.mouse_x(),
            my = go_in_center?disp.height()/2:disp.mouse_y(),
            mX = mx*(width() + (depth()>1?depth():0))/disp.width(),
            mY = my*(height() + (depth()>1?depth():0))/disp.height();
          int X = (int)_XYZ[0], Y = (int)_XYZ[1], Z = (int)_XYZ[2];
          if (mX<width() && mY<height())  {
            X = x0 + mX*(1 + x1 - x0)/width(); Y = y0 + mY*(1 + y1 - y0)/height();
          }
          if (mX<width() && mY>=height()) {
            X = x0 + mX*(1 + x1 - x0)/width(); Z = z0 + (mY - height())*(1 + z1 - z0)/depth();
          }
          if (mX>=width() && mY<height()) {
            Y = y0 + mY*(1 + y1 - y0)/height(); Z = z0 + (mX - width())*(1 + z1 - z0)/depth();
          }
          if (x1 - x0>4) { x0 = X - 3*(X - x0)/4; x1 = X + 3*(x1 - X)/4; }
          if (y1 - y0>4) { y0 = Y - 3*(Y - y0)/4; y1 = Y + 3*(y1 - Y)/4; }
          if (z1 - z0>4) { z0 = Z - 3*(Z - z0)/4; z1 = Z + 3*(z1 - Z)/4; }
        }
        if (go_out) {
          const int
            delta_x = (x1 - x0)/8, delta_y = (y1 - y0)/8, delta_z = (z1 - z0)/8,
            ndelta_x = delta_x?delta_x:(_width>1),
            ndelta_y = delta_y?delta_y:(_height>1),
            ndelta_z = delta_z?delta_z:(_depth>1);
          x0-=ndelta_x; y0-=ndelta_y; z0-=ndelta_z;
          x1+=ndelta_x; y1+=ndelta_y; z1+=ndelta_z;
          if (x0<0) { x1-=x0; x0 = 0; if (x1>=width()) x1 = width() - 1; }
          if (y0<0) { y1-=y0; y0 = 0; if (y1>=height()) y1 = height() - 1; }
          if (z0<0) { z1-=z0; z0 = 0; if (z1>=depth()) z1 = depth() - 1; }
          if (x1>=width()) { x0-=(x1 - width() + 1); x1 = width() - 1; if (x0<0) x0 = 0; }
          if (y1>=height()) { y0-=(y1 - height() + 1); y1 = height() - 1; if (y0<0) y0 = 0; }
          if (z1>=depth()) { z0-=(z1 - depth() + 1); z1 = depth() - 1; if (z0<0) z0 = 0; }
          const float
            ratio = (float)(x1-x0)/(y1-y0),
            ratiow = (float)disp._width/disp._height,
            sub = std::min(cimg::abs(ratio - ratiow),cimg::abs(1/ratio-1/ratiow));
          if (sub>0.01) resize_disp = true;
        }
        if (go_left) {
          const int delta = (x1 - x0)/4, ndelta = delta?delta:(_width>1);
          if (x0 - ndelta>=0) { x0-=ndelta; x1-=ndelta; }
          else { x1-=x0; x0 = 0; }
        }
        if (go_right) {
          const int delta = (x1 - x0)/4, ndelta = delta?delta:(_width>1);
          if (x1+ndelta<width()) { x0+=ndelta; x1+=ndelta; }
          else { x0+=(width() - 1 - x1); x1 = width() - 1; }
        }
        if (go_up) {
          const int delta = (y1 - y0)/4, ndelta = delta?delta:(_height>1);
          if (y0 - ndelta>=0) { y0-=ndelta; y1-=ndelta; }
          else { y1-=y0; y0 = 0; }
        }
        if (go_down) {
          const int delta = (y1 - y0)/4, ndelta = delta?delta:(_height>1);
          if (y1+ndelta<height()) { y0+=ndelta; y1+=ndelta; }
          else { y0+=(height() - 1 - y1); y1 = height() - 1; }
        }
        if (go_inc) {
          const int delta = (z1 - z0)/4, ndelta = delta?delta:(_depth>1);
          if (z0 - ndelta>=0) { z0-=ndelta; z1-=ndelta; }
          else { z1-=z0; z0 = 0; }
        }
        if (go_dec) {
          const int delta = (z1 - z0)/4, ndelta = delta?delta:(_depth>1);
          if (z1+ndelta<depth()) { z0+=ndelta; z1+=ndelta; }
          else { z0+=(depth() - 1 - z1); z1 = depth() - 1; }
        }
        disp.wait(100);
        if (!exit_on_anykey && key && key!=cimg::keyESC &&
            (key!=cimg::keyW || (!disp.is_keyCTRLLEFT() && !disp.is_keyCTRLRIGHT()))) {
          key = 0;
        }
      }
      disp.set_key(key);
      if (XYZ) { XYZ[0] = _XYZ[0]; XYZ[1] = _XYZ[1]; XYZ[2] = _XYZ[2]; }
      return *this;