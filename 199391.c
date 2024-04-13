
    CImg<intT> _select(CImgDisplay &disp, const char *const title,
                       const unsigned int feature_type, unsigned int *const XYZ,
                       const int origX, const int origY, const int origZ,
                       const bool exit_on_anykey,
                       const bool reset_view3d,
                       const bool force_display_z_coord) const {
      if (is_empty()) return CImg<intT>(1,feature_type==0?3:6,1,1,-1);
      if (!disp) {
        disp.assign(cimg_fitscreen(_width,_height,_depth),title?title:0,1);
        if (!title) disp.set_title("CImg<%s> (%ux%ux%ux%u)",pixel_type(),_width,_height,_depth,_spectrum);
      } else if (title) disp.set_title("%s",title);

      CImg<T> thumb;
      if (width()>disp.screen_width() || height()>disp.screen_height())
        get_resize(cimg_fitscreen(width(),height(),depth()),depth(),-100).move_to(thumb);

      const unsigned int old_normalization = disp.normalization();
      bool old_is_resized = disp.is_resized();
      disp._normalization = 0;
      disp.show().set_key(0).set_wheel().show_mouse();

      static const unsigned char foreground_color[] = { 255,255,255 }, background_color[] = { 0,0,0 };

      int area = 0, starting_area = 0, clicked_area = 0, phase = 0,
        X0 = (int)((XYZ?XYZ[0]:(_width - 1)/2)%_width),
        Y0 = (int)((XYZ?XYZ[1]:(_height - 1)/2)%_height),
        Z0 = (int)((XYZ?XYZ[2]:(_depth - 1)/2)%_depth),
        X1 =-1, Y1 = -1, Z1 = -1,
        X3d = -1, Y3d = -1,
        oX3d = X3d, oY3d = -1,
        omx = -1, omy = -1;
      float X = -1, Y = -1, Z = -1;
      unsigned int old_button = 0, key = 0;

      bool shape_selected = false, text_down = false, visible_cursor = true;
      static CImg<floatT> pose3d;
      static bool is_view3d = false, is_axes = true;
      if (reset_view3d) { pose3d.assign(); is_view3d = false; }
      CImg<floatT> points3d, opacities3d, sel_opacities3d;
      CImgList<uintT> primitives3d, sel_primitives3d;
      CImgList<ucharT> colors3d, sel_colors3d;
      CImg<ucharT> visu, visu0, view3d;
      CImg<charT> text(1024); *text = 0;

      while (!key && !disp.is_closed() && !shape_selected) {

        // Handle mouse motion and selection
        int
          mx = disp.mouse_x(),
          my = disp.mouse_y();

        const float
          mX = mx<0?-1.0f:(float)mx*(width() + (depth()>1?depth():0))/disp.width(),
          mY = my<0?-1.0f:(float)my*(height() + (depth()>1?depth():0))/disp.height();

        area = 0;
        if (mX>=0 && mY>=0 && mX<width() && mY<height())  { area = 1; X = mX; Y = mY; Z = (float)(phase?Z1:Z0); }
        if (mX>=0 && mX<width() && mY>=height()) { area = 2; X = mX; Z = mY - _height; Y = (float)(phase?Y1:Y0); }
        if (mY>=0 && mX>=width() && mY<height()) { area = 3; Y = mY; Z = mX - _width; X = (float)(phase?X1:X0); }
        if (mX>=width() && mY>=height()) area = 4;
        if (disp.button()) { if (!clicked_area) clicked_area = area; } else clicked_area = 0;

        CImg<charT> filename(32);

        switch (key = disp.key()) {
#if cimg_OS!=2
        case cimg::keyCTRLRIGHT :
#endif
        case 0 : case cimg::keyCTRLLEFT : key = 0; break;
        case cimg::keyPAGEUP :
          if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { disp.set_wheel(1); key = 0; } break;
        case cimg::keyPAGEDOWN :
          if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) { disp.set_wheel(-1); key = 0; } break;
        case cimg::keyA : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            is_axes = !is_axes; disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyD : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).
              resize(CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,false),
                     CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,true),false).
              _is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyC : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).
              resize(cimg_fitscreen(2*disp.width()/3,2*disp.height()/3,1),false)._is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyR : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).resize(cimg_fitscreen(_width,_height,_depth),false)._is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyF : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.resize(disp.screen_width(),disp.screen_height(),false).toggle_fullscreen()._is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyV : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            is_view3d = !is_view3d; disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyS : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            static unsigned int snap_number = 0;
            std::FILE *file;
            do {
              cimg_snprintf(filename,filename._width,cimg_appname "_%.4u.bmp",snap_number++);
              if ((file=std_fopen(filename,"r"))!=0) cimg::fclose(file);
            } while (file);
            if (visu0) {
              (+visu0).draw_text(0,0," Saving snapshot... ",foreground_color,background_color,0.7f,13).display(disp);
              visu0.save(filename);
              (+visu0).draw_text(0,0," Snapshot '%s' saved. ",foreground_color,background_color,0.7f,13,filename._data).
                display(disp);
            }
            disp.set_key(key,false); key = 0;
          } break;
        case cimg::keyO : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            static unsigned int snap_number = 0;
            std::FILE *file;
            do {
#ifdef cimg_use_zlib
              cimg_snprintf(filename,filename._width,cimg_appname "_%.4u.cimgz",snap_number++);
#else
              cimg_snprintf(filename,filename._width,cimg_appname "_%.4u.cimg",snap_number++);
#endif
              if ((file=std_fopen(filename,"r"))!=0) cimg::fclose(file);
            } while (file);
            (+visu0).draw_text(0,0," Saving instance... ",foreground_color,background_color,0.7f,13).display(disp);
            save(filename);
            (+visu0).draw_text(0,0," Instance '%s' saved. ",foreground_color,background_color,0.7f,13,filename._data).
              display(disp);
            disp.set_key(key,false); key = 0;
          } break;
        }

        switch (area) {

        case 0 : // When mouse is out of image range.
          mx = my = -1; X = Y = Z = -1;
          break;

        case 1 : case 2 : case 3 : // When mouse is over the XY,XZ or YZ projections.
          if (disp.button()&1 && phase<2 && clicked_area==area) { // When selection has been started (1st step).
            if (_depth>1 && (X1!=(int)X || Y1!=(int)Y || Z1!=(int)Z)) visu0.assign();
            X1 = (int)X; Y1 = (int)Y; Z1 = (int)Z;
          }
          if (!(disp.button()&1) && phase>=2 && clicked_area!=area) { // When selection is at 2nd step (for volumes).
            switch (starting_area) {
            case 1 : if (Z1!=(int)Z) visu0.assign(); Z1 = (int)Z; break;
            case 2 : if (Y1!=(int)Y) visu0.assign(); Y1 = (int)Y; break;
            case 3 : if (X1!=(int)X) visu0.assign(); X1 = (int)X; break;
            }
          }
          if (disp.button()&2 && clicked_area==area) { // When moving through the image/volume.
            if (phase) {
              if (_depth>1 && (X1!=(int)X || Y1!=(int)Y || Z1!=(int)Z)) visu0.assign();
              X1 = (int)X; Y1 = (int)Y; Z1 = (int)Z;
            } else {
              if (_depth>1 && (X0!=(int)X || Y0!=(int)Y || Z0!=(int)Z)) visu0.assign();
              X0 = (int)X; Y0 = (int)Y; Z0 = (int)Z;
            }
          }
          if (disp.button()&4) {
            X = (float)X0; Y = (float)Y0; Z = (float)Z0; phase = area = clicked_area = starting_area = 0;
            visu0.assign();
          }
          if (disp.wheel()) { // When moving through the slices of the volume (with mouse wheel).
            if (_depth>1 && !disp.is_keyCTRLLEFT() && !disp.is_keyCTRLRIGHT() &&
                !disp.is_keySHIFTLEFT() && !disp.is_keySHIFTRIGHT()) {
              switch (area) {
              case 1 :
                if (phase) Z = (float)(Z1+=disp.wheel()); else Z = (float)(Z0+=disp.wheel());
                visu0.assign(); break;
              case 2 :
                if (phase) Y = (float)(Y1+=disp.wheel()); else Y = (float)(Y0+=disp.wheel());
                visu0.assign(); break;
              case 3 :
                if (phase) X = (float)(X1+=disp.wheel()); else X = (float)(X0+=disp.wheel());
                visu0.assign(); break;
              }
              disp.set_wheel();
            } else key = ~0U;
          }
          if ((disp.button()&1)!=old_button) { // When left button has just been pressed or released.
            switch (phase) {
            case 0 :
              if (area==clicked_area) {
                X0 = X1 = (int)X; Y0 = Y1 = (int)Y; Z0 = Z1 = (int)Z; starting_area = area; ++phase;
              } break;
            case 1 :
              if (area==starting_area) {
                X1 = (int)X; Y1 = (int)Y; Z1 = (int)Z; ++phase;
              } else if (!(disp.button()&1)) { X = (float)X0; Y = (float)Y0; Z = (float)Z0; phase = 0; visu0.assign(); }
              break;
            case 2 : ++phase; break;
            }
            old_button = disp.button()&1;
          }
          break;

        case 4 : // When mouse is over the 3d view.
          if (is_view3d && points3d) {
            X3d = mx - width()*disp.width()/(width() + (depth()>1?depth():0));
            Y3d = my - height()*disp.height()/(height() + (depth()>1?depth():0));
            if (oX3d<0) { oX3d = X3d; oY3d = Y3d; }
            // Left + right buttons: reset.
            if ((disp.button()&3)==3) { pose3d.assign(); view3d.assign(); oX3d = oY3d = X3d = Y3d = -1; }
            else if (disp.button()&1 && pose3d && (oX3d!=X3d || oY3d!=Y3d)) { // Left button: rotate.
              const float
                R = 0.45f*std::min(view3d._width,view3d._height),
                R2 = R*R,
                u0 = (float)(oX3d - view3d.width()/2),
                v0 = (float)(oY3d - view3d.height()/2),
                u1 = (float)(X3d - view3d.width()/2),
                v1 = (float)(Y3d - view3d.height()/2),
                n0 = cimg::hypot(u0,v0),
                n1 = cimg::hypot(u1,v1),
                nu0 = n0>R?(u0*R/n0):u0,
                nv0 = n0>R?(v0*R/n0):v0,
                nw0 = (float)std::sqrt(std::max(0.0f,R2 - nu0*nu0 - nv0*nv0)),
                nu1 = n1>R?(u1*R/n1):u1,
                nv1 = n1>R?(v1*R/n1):v1,
                nw1 = (float)std::sqrt(std::max(0.0f,R2 - nu1*nu1 - nv1*nv1)),
                u = nv0*nw1 - nw0*nv1,
                v = nw0*nu1 - nu0*nw1,
                w = nv0*nu1 - nu0*nv1,
                n = cimg::hypot(u,v,w),
                alpha = (float)std::asin(n/R2)*180/cimg::PI;
              pose3d.draw_image(CImg<floatT>::rotation_matrix(u,v,w,-alpha)*pose3d.get_crop(0,0,2,2));
              view3d.assign();
            } else if (disp.button()&2 && pose3d && oY3d!=Y3d) {  // Right button: zoom.
              pose3d(3,2)-=(oY3d - Y3d)*1.5f; view3d.assign();
            }
            if (disp.wheel()) { // Wheel: zoom
              pose3d(3,2)-=disp.wheel()*15; view3d.assign(); disp.set_wheel();
            }
            if (disp.button()&4 && pose3d && (oX3d!=X3d || oY3d!=Y3d)) { // Middle button: shift.
              pose3d(3,0)-=oX3d - X3d; pose3d(3,1)-=oY3d - Y3d; view3d.assign();
            }
            oX3d = X3d; oY3d = Y3d;
          }
          mx = my = -1; X = Y = Z = -1;
          break;
        }

        if (phase) {
          if (!feature_type) shape_selected = phase?true:false;
          else {
            if (_depth>1) shape_selected = (phase==3)?true:false;
            else shape_selected = (phase==2)?true:false;
          }
        }

        if (X0<0) X0 = 0;
        if (X0>=width()) X0 = width() - 1;
        if (Y0<0) Y0 = 0;
        if (Y0>=height()) Y0 = height() - 1;
        if (Z0<0) Z0 = 0;
        if (Z0>=depth()) Z0 = depth() - 1;
        if (X1<1) X1 = 0;
        if (X1>=width()) X1 = width() - 1;
        if (Y1<0) Y1 = 0;
        if (Y1>=height()) Y1 = height() - 1;
        if (Z1<0) Z1 = 0;
        if (Z1>=depth()) Z1 = depth() - 1;

        // Draw visualization image on the display
        if (mx!=omx || my!=omy || !visu0 || (_depth>1 && !view3d)) {

          if (!visu0) { // Create image of projected planes.
            if (thumb) thumb.__get_select(disp,old_normalization,phase?X1:X0,phase?Y1:Y0,phase?Z1:Z0).move_to(visu0);
            else __get_select(disp,old_normalization,phase?X1:X0,phase?Y1:Y0,phase?Z1:Z0).move_to(visu0);
            visu0.resize(disp);
            view3d.assign();
            points3d.assign();
          }

          if (is_view3d && _depth>1 && !view3d) { // Create 3d view for volumetric images.
            const unsigned int
              _x3d = (unsigned int)cimg::round((float)_width*visu0._width/(_width + _depth),1,1),
              _y3d = (unsigned int)cimg::round((float)_height*visu0._height/(_height + _depth),1,1),
              x3d = _x3d>=visu0._width?visu0._width - 1:_x3d,
              y3d = _y3d>=visu0._height?visu0._height - 1:_y3d;
            CImg<ucharT>(1,2,1,1,64,128).resize(visu0._width - x3d,visu0._height - y3d,1,visu0._spectrum,3).
              move_to(view3d);
            if (!points3d) {
              get_projections3d(primitives3d,colors3d,phase?X1:X0,phase?Y1:Y0,phase?Z1:Z0,true).move_to(points3d);
              points3d.append(CImg<floatT>(8,3,1,1,
                                           0,_width - 1,_width - 1,0,0,_width - 1,_width - 1,0,
                                           0,0,_height - 1,_height - 1,0,0,_height - 1,_height - 1,
                                           0,0,0,0,_depth - 1,_depth - 1,_depth - 1,_depth - 1),'x');
              CImg<uintT>::vector(12,13).move_to(primitives3d); CImg<uintT>::vector(13,14).move_to(primitives3d);
              CImg<uintT>::vector(14,15).move_to(primitives3d); CImg<uintT>::vector(15,12).move_to(primitives3d);
              CImg<uintT>::vector(16,17).move_to(primitives3d); CImg<uintT>::vector(17,18).move_to(primitives3d);
              CImg<uintT>::vector(18,19).move_to(primitives3d); CImg<uintT>::vector(19,16).move_to(primitives3d);
              CImg<uintT>::vector(12,16).move_to(primitives3d); CImg<uintT>::vector(13,17).move_to(primitives3d);
              CImg<uintT>::vector(14,18).move_to(primitives3d); CImg<uintT>::vector(15,19).move_to(primitives3d);
              colors3d.insert(12,CImg<ucharT>::vector(255,255,255));
              opacities3d.assign(primitives3d.width(),1,1,1,0.5f);
              if (!phase) {
                opacities3d[0] = opacities3d[1] = opacities3d[2] = 0.8f;
                sel_primitives3d.assign();
                sel_colors3d.assign();
                sel_opacities3d.assign();
              } else {
                if (feature_type==2) {
                  points3d.append(CImg<floatT>(8,3,1,1,
                                               X0,X1,X1,X0,X0,X1,X1,X0,
                                               Y0,Y0,Y1,Y1,Y0,Y0,Y1,Y1,
                                               Z0,Z0,Z0,Z0,Z1,Z1,Z1,Z1),'x');
                  sel_primitives3d.assign();
                  CImg<uintT>::vector(20,21).move_to(sel_primitives3d);
                  CImg<uintT>::vector(21,22).move_to(sel_primitives3d);
                  CImg<uintT>::vector(22,23).move_to(sel_primitives3d);
                  CImg<uintT>::vector(23,20).move_to(sel_primitives3d);
                  CImg<uintT>::vector(24,25).move_to(sel_primitives3d);
                  CImg<uintT>::vector(25,26).move_to(sel_primitives3d);
                  CImg<uintT>::vector(26,27).move_to(sel_primitives3d);
                  CImg<uintT>::vector(27,24).move_to(sel_primitives3d);
                  CImg<uintT>::vector(20,24).move_to(sel_primitives3d);
                  CImg<uintT>::vector(21,25).move_to(sel_primitives3d);
                  CImg<uintT>::vector(22,26).move_to(sel_primitives3d);
                  CImg<uintT>::vector(23,27).move_to(sel_primitives3d);
                } else {
                  points3d.append(CImg<floatT>(2,3,1,1,
                                               X0,X1,
                                               Y0,Y1,
                                               Z0,Z1),'x');
                  sel_primitives3d.assign(CImg<uintT>::vector(20,21));
                }
                sel_colors3d.assign(sel_primitives3d._width,CImg<ucharT>::vector(255,255,255));
                sel_opacities3d.assign(sel_primitives3d._width,1,1,1,0.8f);
              }
              points3d.shift_object3d(-0.5f*(_width - 1),-0.5f*(_height - 1),-0.5f*(_depth - 1)).resize_object3d();
              points3d*=0.75f*std::min(view3d._width,view3d._height);
            }

            if (!pose3d) CImg<floatT>(4,3,1,1, 1,0,0,0, 0,1,0,0, 0,0,1,0).move_to(pose3d);
            CImg<floatT> zbuffer3d(view3d._width,view3d._height,1,1,0);
            const CImg<floatT> rotated_points3d = pose3d.get_crop(0,0,2,2)*points3d;
            if (sel_primitives3d)
              view3d.draw_object3d(pose3d(3,0) + 0.5f*view3d._width,
                                   pose3d(3,1) + 0.5f*view3d._height,
                                   pose3d(3,2),
                                   rotated_points3d,sel_primitives3d,sel_colors3d,sel_opacities3d,
                                   2,true,500,0,0,0,0,0,zbuffer3d);
            view3d.draw_object3d(pose3d(3,0) + 0.5f*view3d._width,
                                 pose3d(3,1) + 0.5f*view3d._height,
                                 pose3d(3,2),
                                 rotated_points3d,primitives3d,colors3d,opacities3d,
                                 2,true,500,0,0,0,0,0,zbuffer3d);
            visu0.draw_image(x3d,y3d,view3d);
          }
          visu = visu0;

          if (X<0 || Y<0 || Z<0) { if (!visible_cursor) { disp.show_mouse(); visible_cursor = true; }}
          else {
            if (is_axes) { if (visible_cursor) { disp.hide_mouse(); visible_cursor = false; }}
            else { if (!visible_cursor) { disp.show_mouse(); visible_cursor = true; }}
            const int d = (depth()>1)?depth():0;
            int
              _vX = (int)X, _vY = (int)Y, _vZ = (int)Z,
              w = disp.width(), W = width() + d,
              h = disp.height(), H = height() + d,
              _xp = (int)(_vX*(float)w/W), xp = _xp + ((int)(_xp*(float)W/w)!=_vX),
              _yp = (int)(_vY*(float)h/H), yp = _yp + ((int)(_yp*(float)H/h)!=_vY),
              _xn = (int)((_vX + 1.0f)*w/W - 1), xn = _xn + ((int)((_xn + 1.0f)*W/w)!=_vX + 1),
              _yn = (int)((_vY + 1.0f)*h/H - 1), yn = _yn + ((int)((_yn + 1.0f)*H/h)!=_vY + 1),
              _zxp = (int)((_vZ + width())*(float)w/W), zxp = _zxp + ((int)(_zxp*(float)W/w)!=_vZ + width()),
              _zyp = (int)((_vZ + height())*(float)h/H), zyp = _zyp + ((int)(_zyp*(float)H/h)!=_vZ + height()),
              _zxn = (int)((_vZ + width() + 1.0f)*w/W - 1),
                       zxn = _zxn + ((int)((_zxn + 1.0f)*W/w)!=_vZ + width() + 1),
              _zyn = (int)((_vZ + height() + 1.0f)*h/H - 1),
                       zyn = _zyn + ((int)((_zyn + 1.0f)*H/h)!=_vZ + height() + 1),
              _xM = (int)(width()*(float)w/W - 1), xM = _xM + ((int)((_xM + 1.0f)*W/w)!=width()),
              _yM = (int)(height()*(float)h/H - 1), yM = _yM + ((int)((_yM + 1.0f)*H/h)!=height()),
              xc = (xp + xn)/2,
              yc = (yp + yn)/2,
              zxc = (zxp + zxn)/2,
              zyc = (zyp + zyn)/2,
              xf = (int)(X*w/W),
              yf = (int)(Y*h/H),
              zxf = (int)((Z + width())*w/W),
              zyf = (int)((Z + height())*h/H);

            if (is_axes) { // Draw axes.
              visu.draw_line(0,yf,visu.width() - 1,yf,foreground_color,0.7f,0xFF00FF00).
                draw_line(0,yf,visu.width() - 1,yf,background_color,0.7f,0x00FF00FF).
                draw_line(xf,0,xf,visu.height() - 1,foreground_color,0.7f,0xFF00FF00).
                draw_line(xf,0,xf,visu.height() - 1,background_color,0.7f,0x00FF00FF);
              if (_depth>1)
                visu.draw_line(zxf,0,zxf,yM,foreground_color,0.7f,0xFF00FF00).
                  draw_line(zxf,0,zxf,yM,background_color,0.7f,0x00FF00FF).
                  draw_line(0,zyf,xM,zyf,foreground_color,0.7f,0xFF00FF00).
                  draw_line(0,zyf,xM,zyf,background_color,0.7f,0x00FF00FF);
            }

            // Draw box cursor.
            if (xn - xp>=4 && yn - yp>=4) visu.draw_rectangle(xp,yp,xn,yn,foreground_color,0.2f).
                                        draw_rectangle(xp,yp,xn,yn,foreground_color,1,0xAAAAAAAA).
                                        draw_rectangle(xp,yp,xn,yn,background_color,1,0x55555555);
            if (_depth>1) {
              if (yn - yp>=4 && zxn - zxp>=4) visu.draw_rectangle(zxp,yp,zxn,yn,background_color,0.2f).
                                            draw_rectangle(zxp,yp,zxn,yn,foreground_color,1,0xAAAAAAAA).
                                            draw_rectangle(zxp,yp,zxn,yn,background_color,1,0x55555555);
              if (xn - xp>=4 && zyn - zyp>=4) visu.draw_rectangle(xp,zyp,xn,zyn,background_color,0.2f).
                                            draw_rectangle(xp,zyp,xn,zyn,foreground_color,1,0xAAAAAAAA).
                                            draw_rectangle(xp,zyp,xn,zyn,background_color,1,0x55555555);
            }

            // Draw selection.
            if (phase) {
              const int
                _xp0 = (int)(X0*(float)w/W), xp0 = _xp0 + ((int)(_xp0*(float)W/w)!=X0),
                _yp0 = (int)(Y0*(float)h/H), yp0 = _yp0 + ((int)(_yp0*(float)H/h)!=Y0),
                _xn0 = (int)((X0 + 1.0f)*w/W - 1), xn0 = _xn0 + ((int)((_xn0 + 1.0f)*W/w)!=X0 + 1),
                _yn0 = (int)((Y0 + 1.0f)*h/H - 1), yn0 = _yn0 + ((int)((_yn0 + 1.0f)*H/h)!=Y0 + 1),
                _zxp0 = (int)((Z0 + width())*(float)w/W), zxp0 = _zxp0 + ((int)(_zxp0*(float)W/w)!=Z0 + width()),
                _zyp0 = (int)((Z0 + height())*(float)h/H), zyp0 = _zyp0 + ((int)(_zyp0*(float)H/h)!=Z0 + height()),
                _zxn0 = (int)((Z0 + width() + 1.0f)*w/W - 1),
                zxn0 = _zxn0 + ((int)((_zxn0 + 1.0f)*W/w)!=Z0 + width() + 1),
                _zyn0 = (int)((Z0 + height() + 1.0f)*h/H - 1),
                zyn0 = _zyn0 + ((int)((_zyn0 + 1.0f)*H/h)!=Z0 + height() + 1),
                xc0 = (xp0 + xn0)/2,
                yc0 = (yp0 + yn0)/2,
                zxc0 = (zxp0 + zxn0)/2,
                zyc0 = (zyp0 + zyn0)/2;

              switch (feature_type) {
              case 1 : {
                visu.draw_arrow(xc0,yc0,xc,yc,background_color,0.9f,30,5,0x55555555).
                  draw_arrow(xc0,yc0,xc,yc,foreground_color,0.9f,30,5,0xAAAAAAAA);
                if (d) {
                  visu.draw_arrow(zxc0,yc0,zxc,yc,background_color,0.9f,30,5,0x55555555).
                    draw_arrow(zxc0,yc0,zxc,yc,foreground_color,0.9f,30,5,0xAAAAAAAA).
                    draw_arrow(xc0,zyc0,xc,zyc,background_color,0.9f,30,5,0x55555555).
                    draw_arrow(xc0,zyc0,xc,zyc,foreground_color,0.9f,30,5,0xAAAAAAAA);
                }
              } break;
              case 2 : {
                visu.draw_rectangle(X0<X1?xp0:xp,Y0<Y1?yp0:yp,X0<X1?xn:xn0,Y0<Y1?yn:yn0,background_color,0.2f).
                  draw_rectangle(X0<X1?xp0:xp,Y0<Y1?yp0:yp,X0<X1?xn:xn0,Y0<Y1?yn:yn0,background_color,0.9f,0x55555555).
                  draw_rectangle(X0<X1?xp0:xp,Y0<Y1?yp0:yp,X0<X1?xn:xn0,Y0<Y1?yn:yn0,foreground_color,0.9f,0xAAAAAAAA).
                  draw_arrow(xc0,yc0,xc,yc,background_color,0.5f,30,5,0x55555555).
                  draw_arrow(xc0,yc0,xc,yc,foreground_color,0.5f,30,5,0xAAAAAAAA);
                if (d) {
                  visu.draw_rectangle(Z0<Z1?zxp0:zxp,Y0<Y1?yp0:yp,Z0<Z1?zxn:zxn0,Y0<Y1?yn:yn0,background_color,0.2f).
                    draw_rectangle(Z0<Z1?zxp0:zxp,Y0<Y1?yp0:yp,Z0<Z1?zxn:zxn0,Y0<Y1?yn:yn0,
                                   background_color,0.9f,0x55555555).
                    draw_rectangle(Z0<Z1?zxp0:zxp,Y0<Y1?yp0:yp,Z0<Z1?zxn:zxn0,Y0<Y1?yn:yn0,
                                   foreground_color,0.9f,0xAAAAAAAA).
                    draw_arrow(zxc0,yc0,zxc,yc,background_color,0.5f,30,5,0x55555555).
                    draw_arrow(zxc0,yc0,zxc,yc,foreground_color,0.5f,30,5,0xAAAAAAAA).
                    draw_rectangle(X0<X1?xp0:xp,Z0<Z1?zyp0:zyp,X0<X1?xn:xn0,Z0<Z1?zyn:zyn0,
                                   background_color,0.2f).
                    draw_rectangle(X0<X1?xp0:xp,Z0<Z1?zyp0:zyp,X0<X1?xn:xn0,Z0<Z1?zyn:zyn0,
                                   background_color,0.9f,0x55555555).
                    draw_rectangle(X0<X1?xp0:xp,Z0<Z1?zyp0:zyp,X0<X1?xn:xn0,Z0<Z1?zyn:zyn0,
                                   foreground_color,0.9f,0xAAAAAAAA).
                    draw_arrow(xp0,zyp0,xn,zyn,background_color,0.5f,30,5,0x55555555).
                    draw_arrow(xp0,zyp0,xn,zyn,foreground_color,0.5f,30,5,0xAAAAAAAA);
                }
              } break;
              case 3 : {
                visu.draw_ellipse(xc0,yc0,
                                  (float)cimg::abs(xc - xc0),
                                  (float)cimg::abs(yc - yc0),0,background_color,0.2f).
                  draw_ellipse(xc0,yc0,
                               (float)cimg::abs(xc - xc0),
                               (float)cimg::abs(yc - yc0),0,foreground_color,0.9f,~0U).
                  draw_point(xc0,yc0,foreground_color,0.9f);
                if (d) {
                  visu.draw_ellipse(zxc0,yc0,(float)cimg::abs(zxc - zxc0),(float)cimg::abs(yc - yc0),0,
                                    background_color,0.2f).
                    draw_ellipse(zxc0,yc0,(float)cimg::abs(zxc - zxc0),(float)cimg::abs(yc - yc0),0,
                                 foreground_color,0.9f,~0U).
                    draw_point(zxc0,yc0,foreground_color,0.9f).
                    draw_ellipse(xc0,zyc0,(float)cimg::abs(xc - xc0),(float)cimg::abs(zyc - zyc0),0,
                                 background_color,0.2f).
                    draw_ellipse(xc0,zyc0,(float)cimg::abs(xc - xc0),(float)cimg::abs(zyc - zyc0),0,
                                 foreground_color,0.9f,~0U).
                    draw_point(xc0,zyc0,foreground_color,0.9f);
                }
              } break;
              }
            }

            // Draw text info.
            if (my>=0 && my<13) text_down = true; else if (my>=visu.height() - 13) text_down = false;
            if (!feature_type || !phase) {
              if (X>=0 && Y>=0 && Z>=0 && X<width() && Y<height() && Z<depth()) {
                if (_depth>1 || force_display_z_coord)
                  cimg_snprintf(text,text._width," Point (%d,%d,%d) = [ ",origX + (int)X,origY + (int)Y,origZ + (int)Z);
                else cimg_snprintf(text,text._width," Point (%d,%d) = [ ",origX + (int)X,origY + (int)Y);
                CImg<T> values = get_vector_at(X,Y,Z);
                const bool is_large_spectrum = values._height>16;
                if (is_large_spectrum)
                  values.draw_image(0,8,values.get_rows(values._height - 8,values._height - 1)).resize(1,16,1,1,0);
                char *ctext = text._data + std::strlen(text), *const ltext = text._data + 512;
                for (unsigned int c = 0; c<values._height && ctext<ltext; ++c) {
                  cimg_snprintf(ctext,24,cimg::type<T>::format_s(),
                                cimg::type<T>::format(values[c]));
                  ctext += std::strlen(ctext);
                  if (c==7 && is_large_spectrum) {
                    cimg_snprintf(ctext,24," (...)");
                    ctext += std::strlen(ctext);
                  }
                  *(ctext++) = ' '; *ctext = 0;
                }
                std::strcpy(text._data + std::strlen(text),"] ");
              }
            } else switch (feature_type) {
              case 1 : {
                const double dX = (double)(X0 - X1), dY = (double)(Y0 - Y1), dZ = (double)(Z0 - Z1),
                  length = cimg::hypot(dX,dY,dZ);
                if (_depth>1 || force_display_z_coord)
                  cimg_snprintf(text,text._width," Vect (%d,%d,%d)-(%d,%d,%d), Length = %g ",
                                origX + X0,origY + Y0,origZ + Z0,origX + X1,origY + Y1,origZ + Z1,length);
                else cimg_snprintf(text,text._width," Vect (%d,%d)-(%d,%d), Length = %g ",
                                   origX + X0,origY + Y0,origX + X1,origY + Y1,length);
              } break;
              case 2 : {
                const double dX = (double)(X0 - X1), dY = (double)(Y0 - Y1), dZ = (double)(Z0 - Z1),
                  length = cimg::hypot(dX,dY,dZ);
                if (_depth>1 || force_display_z_coord)
                  cimg_snprintf(text,text._width," Box (%d,%d,%d)-(%d,%d,%d), Size = (%d,%d,%d), Length = %g ",
                                origX + (X0<X1?X0:X1),origY + (Y0<Y1?Y0:Y1),origZ + (Z0<Z1?Z0:Z1),
                                origX + (X0<X1?X1:X0),origY + (Y0<Y1?Y1:Y0),origZ + (Z0<Z1?Z1:Z0),
                                1 + cimg::abs(X0 - X1),1 + cimg::abs(Y0 - Y1),1 + cimg::abs(Z0 - Z1),length);
                else cimg_snprintf(text,text._width," Box (%d,%d)-(%d,%d), Size = (%d,%d), Length = %g ",
                                   origX + (X0<X1?X0:X1),origY + (Y0<Y1?Y0:Y1),
                                   origX + (X0<X1?X1:X0),origY + (Y0<Y1?Y1:Y0),
                                   1 + cimg::abs(X0 - X1),1 + cimg::abs(Y0 - Y1),length);
              } break;
              default :
                if (_depth>1 || force_display_z_coord)
                  cimg_snprintf(text,text._width," Ellipse (%d,%d,%d)-(%d,%d,%d), Radii = (%d,%d,%d) ",
                                origX + X0,origY + Y0,origZ + Z0,origX + X1,origY + Y1,origZ + Z1,
                                1 + cimg::abs(X0 - X1),1 + cimg::abs(Y0 - Y1),1 + cimg::abs(Z0 - Z1));
                else cimg_snprintf(text,text._width," Ellipse (%d,%d)-(%d,%d), Radii = (%d,%d) ",
                                   origX + X0,origY + Y0,origX + X1,origY + Y1,
                                   1 + cimg::abs(X0 - X1),1 + cimg::abs(Y0 - Y1));
              }
            if (phase || (mx>=0 && my>=0))
              visu.draw_text(0,text_down?visu.height() - 13:0,text,foreground_color,background_color,0.7f,13);
          }

          disp.display(visu).wait();
        } else if (!shape_selected) disp.wait();
        if (disp.is_resized()) { disp.resize(false)._is_resized = false; old_is_resized = true; visu0.assign(); }
        omx = mx; omy = my;
        if (!exit_on_anykey && key && key!=cimg::keyESC &&
            (key!=cimg::keyW || (!disp.is_keyCTRLLEFT() && !disp.is_keyCTRLRIGHT()))) {
          key = 0;
        }
      }

      // Return result.
      CImg<intT> res(1,feature_type==0?3:6,1,1,-1);
      if (XYZ) { XYZ[0] = (unsigned int)X0; XYZ[1] = (unsigned int)Y0; XYZ[2] = (unsigned int)Z0; }
      if (shape_selected) {
        if (feature_type==2) {
          if (X0>X1) cimg::swap(X0,X1);
          if (Y0>Y1) cimg::swap(Y0,Y1);
          if (Z0>Z1) cimg::swap(Z0,Z1);
        }
	if (X1<0 || Y1<0 || Z1<0) X0 = Y0 = Z0 = X1 = Y1 = Z1 = -1;
	switch (feature_type) {
	case 1 : case 2 : res[0] = X0; res[1] = Y0; res[2] = Z0; res[3] = X1; res[4] = Y1; res[5] = Z1; break;
        case 3 :
          res[3] = cimg::abs(X1 - X0); res[4] = cimg::abs(Y1 - Y0); res[5] = cimg::abs(Z1 - Z0);
          res[0] = X0; res[1] = Y0; res[2] = Z0;
          break;
	default : res[0] = X0; res[1] = Y0; res[2] = Z0;
	}
      }
      if (!exit_on_anykey || !(disp.button()&4)) disp.set_button();
      if (!visible_cursor) disp.show_mouse();
      disp._normalization = old_normalization;
      disp._is_resized = old_is_resized;
      if (key!=~0U) disp.set_key(key);
      return res;