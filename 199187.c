      CImgDisplay disp;
      return _select(disp,title,feature_type,axis,align,exit_on_anykey,0,false,false,false);
    }

    CImg<intT> _select(CImgDisplay &disp, const char *const title, const bool feature_type,
                       const char axis, const float align, const bool exit_on_anykey,
                       const unsigned int orig, const bool resize_disp,
                       const bool exit_on_rightbutton, const bool exit_on_wheel) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "select(): Empty instance.",
                                    cimglist_instance);

      // Create image correspondence table and get list dimensions for visualization.
      CImgList<uintT> _indices;
      unsigned int max_width = 0, max_height = 0, sum_width = 0, sum_height = 0;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        const unsigned int
          w = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,false),
          h = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,true);
        if (w>max_width) max_width = w;
        if (h>max_height) max_height = h;
        sum_width+=w; sum_height+=h;
        if (axis=='x') CImg<uintT>(w,1,1,1,(unsigned int)l).move_to(_indices);
        else CImg<uintT>(h,1,1,1,(unsigned int)l).move_to(_indices);
      }
      const CImg<uintT> indices0 = _indices>'x';

      // Create display window.
      if (!disp) {
        if (axis=='x') disp.assign(cimg_fitscreen(sum_width,max_height,1),title?title:0,1);
        else disp.assign(cimg_fitscreen(max_width,sum_height,1),title?title:0,1);
        if (!title) disp.set_title("CImgList<%s> (%u)",pixel_type(),_width);
      } else if (title) disp.set_title("%s",title);
      if (resize_disp) {
        if (axis=='x') disp.resize(cimg_fitscreen(sum_width,max_height,1),false);
        else disp.resize(cimg_fitscreen(max_width,sum_height,1),false);
      }

      const unsigned int old_normalization = disp.normalization();
      bool old_is_resized = disp.is_resized();
      disp._normalization = 0;
      disp.show().set_key(0);
      static const unsigned char foreground_color[] = { 255,255,255 }, background_color[] = { 0,0,0 };

      // Enter event loop.
      CImg<ucharT> visu0, visu;
      CImg<uintT> indices;
      CImg<intT> positions(_width,4,1,1,-1);
      int oindice0 = -1, oindice1 = -1, indice0 = -1, indice1 = -1;
      bool is_clicked = false, is_selected = false, text_down = false, update_display = true;
      unsigned int key = 0;

      while (!is_selected && !disp.is_closed() && !key) {

        // Create background image.
        if (!visu0) {
          visu0.assign(disp._width,disp._height,1,3,0); visu.assign();
          (indices0.get_resize(axis=='x'?visu0._width:visu0._height,1)).move_to(indices);
          unsigned int ind = 0;
          const CImg<T> onexone(1,1,1,1,(T)0);
          if (axis=='x')
            cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=4))
            cimglist_for(*this,ind) {
              unsigned int x0 = 0;
              while (x0<visu0._width && indices[x0++]!=(unsigned int)ind) {}
              unsigned int x1 = x0;
              while (x1<visu0._width && indices[x1++]==(unsigned int)ind) {}
              const CImg<T> &src = _data[ind]?_data[ind]:onexone;
              CImg<ucharT> res;
              src.__get_select(disp,old_normalization,(src._width - 1)/2,(src._height - 1)/2,(src._depth - 1)/2).
                move_to(res);
              const unsigned int h = CImgDisplay::_fitscreen(res._width,res._height,1,128,-85,true);
              res.resize(x1 - x0,std::max(32U,h*disp._height/max_height),1,res._spectrum==1?3:-100);
              positions(ind,0) = positions(ind,2) = (int)x0;
              positions(ind,1) = positions(ind,3) = (int)(align*(visu0.height() - res.height()));
              positions(ind,2)+=res._width;
              positions(ind,3)+=res._height - 1;
              visu0.draw_image(positions(ind,0),positions(ind,1),res);
            }
          else
            cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=4))
            cimglist_for(*this,ind) {
              unsigned int y0 = 0;
              while (y0<visu0._height && indices[y0++]!=(unsigned int)ind) {}
              unsigned int y1 = y0;
              while (y1<visu0._height && indices[y1++]==(unsigned int)ind) {}
              const CImg<T> &src = _data[ind]?_data[ind]:onexone;
              CImg<ucharT> res;
              src.__get_select(disp,old_normalization,(src._width - 1)/2,(src._height - 1)/2,(src._depth - 1)/2).
                move_to(res);
              const unsigned int w = CImgDisplay::_fitscreen(res._width,res._height,1,128,-85,false);
              res.resize(std::max(32U,w*disp._width/max_width),y1 - y0,1,res._spectrum==1?3:-100);
              positions(ind,0) = positions(ind,2) = (int)(align*(visu0.width() - res.width()));
              positions(ind,1) = positions(ind,3) = (int)y0;
              positions(ind,2)+=res._width - 1;
              positions(ind,3)+=res._height;
              visu0.draw_image(positions(ind,0),positions(ind,1),res);
            }
          if (axis=='x') --positions(ind,2); else --positions(ind,3);
          update_display = true;
        }

        if (!visu || oindice0!=indice0 || oindice1!=indice1) {
          if (indice0>=0 && indice1>=0) {
            visu.assign(visu0,false);
            const int indm = std::min(indice0,indice1), indM = std::max(indice0,indice1);
            for (int ind = indm; ind<=indM; ++ind) if (positions(ind,0)>=0) {
                visu.draw_rectangle(positions(ind,0),positions(ind,1),positions(ind,2),positions(ind,3),
                                    background_color,0.2f);
                if ((axis=='x' && positions(ind,2) - positions(ind,0)>=8) ||
                    (axis!='x' && positions(ind,3) - positions(ind,1)>=8))
                  visu.draw_rectangle(positions(ind,0),positions(ind,1),positions(ind,2),positions(ind,3),
                                      foreground_color,0.9f,0xAAAAAAAA);
              }
            const int yt = (int)text_down?visu.height() - 13:0;
            if (is_clicked) visu.draw_text(0,yt," Images #%u - #%u, Size = %u",
                                           foreground_color,background_color,0.7f,13,
                                           orig + indm,orig + indM,indM - indm + 1);
            else visu.draw_text(0,yt," Image #%u (%u,%u,%u,%u)",foreground_color,background_color,0.7f,13,
                                orig + indice0,
                                _data[indice0]._width,
                                _data[indice0]._height,
                                _data[indice0]._depth,
                                _data[indice0]._spectrum);
            update_display = true;
          } else visu.assign();
        }
        if (!visu) { visu.assign(visu0,true); update_display = true; }
        if (update_display) { visu.display(disp); update_display = false; }
        disp.wait();

        // Manage user events.
        const int xm = disp.mouse_x(), ym = disp.mouse_y();
        int indice = -1;

        if (xm>=0) {
          indice = (int)indices(axis=='x'?xm:ym);
          if (disp.button()&1) {
            if (!is_clicked) { is_clicked = true; oindice0 = indice0; indice0 = indice; }
            oindice1 = indice1; indice1 = indice;
            if (!feature_type) is_selected = true;
          } else {
            if (!is_clicked) { oindice0 = oindice1 = indice0; indice0 = indice1 = indice; }
            else is_selected = true;
          }
        } else {
          if (is_clicked) {
            if (!(disp.button()&1)) { is_clicked = is_selected = false; indice0 = indice1 = -1; }
            else indice1 = -1;
          } else indice0 = indice1 = -1;
        }

        if (disp.button()&4) { is_clicked = is_selected = false; indice0 = indice1 = -1; }
        if (disp.button()&2 && exit_on_rightbutton) { is_selected = true; indice1 = indice0 = -1; }
        if (disp.wheel() && exit_on_wheel) is_selected = true;

        CImg<charT> filename(32);
        switch (key = disp.key()) {
#if cimg_OS!=2
        case cimg::keyCTRLRIGHT :
#endif
        case 0 : case cimg::keyCTRLLEFT : key = 0; break;
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
            disp.set_fullscreen(false).
              resize(cimg_fitscreen(axis=='x'?sum_width:max_width,axis=='x'?max_height:sum_height,1),false).
              _is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyF : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.resize(disp.screen_width(),disp.screen_height(),false).toggle_fullscreen()._is_resized = true;
            disp.set_key(key,false); key = 0; visu0.assign();
          } break;
        case cimg::keyS : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            static unsigned int snap_number = 0;
            std::FILE *file;
            do {
              cimg_snprintf(filename,filename._width,cimg_appname "_%.4u.bmp",snap_number++);
              if ((file=std_fopen(filename,"r"))!=0) cimg::fclose(file);
            } while (file);
            if (visu0) {
              (+visu0).draw_text(0,0," Saving snapshot... ",
                                 foreground_color,background_color,0.7f,13).display(disp);
              visu0.save(filename);
              (+visu0).draw_text(0,0," Snapshot '%s' saved. ",
                                 foreground_color,background_color,0.7f,13,filename._data).display(disp);
            }
            disp.set_key(key,false).wait(); key = 0;
          } break;
        case cimg::keyO :
          if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
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
            (+visu0).draw_text(0,0," Saving instance... ",
                               foreground_color,background_color,0.7f,13).display(disp);
            save(filename);
            (+visu0).draw_text(0,0," Instance '%s' saved. ",
                               foreground_color,background_color,0.7f,13,filename._data).display(disp);
            disp.set_key(key,false).wait(); key = 0;
          } break;
        }
        if (disp.is_resized()) { disp.resize(false); visu0.assign(); }
        if (ym>=0 && ym<13) { if (!text_down) { visu.assign(); text_down = true; }}
        else if (ym>=visu.height() - 13) { if (text_down) { visu.assign(); text_down = false; }}
        if (!exit_on_anykey && key && key!=cimg::keyESC &&
            (key!=cimg::keyW || (!disp.is_keyCTRLLEFT() && !disp.is_keyCTRLRIGHT()))) {
          key = 0;
        }
      }
      CImg<intT> res(1,2,1,1,-1);
      if (is_selected) {
        if (feature_type) res.fill(std::min(indice0,indice1),std::max(indice0,indice1));
        else res.fill(indice0);
      }
      if (!(disp.button()&2)) disp.set_button();
      disp._normalization = old_normalization;