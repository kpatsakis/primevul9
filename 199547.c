      bool is_exit = false;
      return _display(disp,title,0,display_info,axis,align,XYZ,exit_on_anykey,0,true,is_exit);
    }

    const CImgList<T>& _display(CImgDisplay &disp, const char *const title, const CImgList<charT> *const titles,
                                const bool display_info, const char axis, const float align, unsigned int *const XYZ,
                                const bool exit_on_anykey, const unsigned int orig, const bool is_first_call,
                                bool &is_exit) const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "display(): Empty instance.",
                                    cimglist_instance);
      if (!disp) {
        if (axis=='x') {
          unsigned int sum_width = 0, max_height = 0;
          cimglist_for(*this,l) {
            const CImg<T> &img = _data[l];
            const unsigned int
              w = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,false),
              h = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,true);
            sum_width+=w;
            if (h>max_height) max_height = h;
          }
          disp.assign(cimg_fitscreen(sum_width,max_height,1),title?title:titles?titles->__display()._data:0,1);
        } else {
          unsigned int max_width = 0, sum_height = 0;
          cimglist_for(*this,l) {
            const CImg<T> &img = _data[l];
            const unsigned int
              w = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,false),
              h = CImgDisplay::_fitscreen(img._width,img._height,img._depth,128,-85,true);
            if (w>max_width) max_width = w;
            sum_height+=h;
          }
          disp.assign(cimg_fitscreen(max_width,sum_height,1),title?title:titles?titles->__display()._data:0,1);
        }
        if (!title && !titles) disp.set_title("CImgList<%s> (%u)",pixel_type(),_width);
      } else if (title) disp.set_title("%s",title);
      else if (titles) disp.set_title("%s",titles->__display()._data);
      const CImg<char> dtitle = CImg<char>::string(disp.title());
      if (display_info) print(disp.title());
      disp.show().flush();

      if (_width==1) {
        const unsigned int dw = disp._width, dh = disp._height;
        if (!is_first_call)
          disp.resize(cimg_fitscreen(_data[0]._width,_data[0]._height,_data[0]._depth),false);
        disp.set_title("%s (%ux%ux%ux%u)",
                       dtitle.data(),_data[0]._width,_data[0]._height,_data[0]._depth,_data[0]._spectrum);
        _data[0]._display(disp,0,false,XYZ,exit_on_anykey,!is_first_call);
        if (disp.key()) is_exit = true;
        disp.resize(cimg_fitscreen(dw,dh,1),false).set_title("%s",dtitle.data());
      } else {
        bool disp_resize = !is_first_call;
        while (!disp.is_closed() && !is_exit) {
          const CImg<intT> s = _select(disp,0,true,axis,align,exit_on_anykey,orig,disp_resize,!is_first_call,true);
          disp_resize = true;
          if (s[0]<0 && !disp.wheel()) { // No selections done.
            if (disp.button()&2) { disp.flush(); break; }
            is_exit = true;
          } else if (disp.wheel()) { // Zoom in/out.
            const int wheel = disp.wheel();
            disp.set_wheel();
            if (!is_first_call && wheel<0) break;
            if (wheel>0 && _width>=4) {
              const unsigned int
                delta = std::max(1U,(unsigned int)cimg::round(0.3*_width)),
                ind0 = (unsigned int)std::max(0,s[0] - (int)delta),
                ind1 = (unsigned int)std::min(width() - 1,s[0] + (int)delta);
              if ((ind0!=0 || ind1!=_width - 1) && ind1 - ind0>=3) {
                const CImgList<T> sublist = get_shared_images(ind0,ind1);
                CImgList<charT> t_sublist;
                if (titles) t_sublist = titles->get_shared_images(ind0,ind1);
                sublist._display(disp,0,titles?&t_sublist:0,false,axis,align,XYZ,exit_on_anykey,
                                 orig + ind0,false,is_exit);
              }
            }
          } else if (s[0]!=0 || s[1]!=width() - 1) {
            const CImgList<T> sublist = get_shared_images(s[0],s[1]);
            CImgList<charT> t_sublist;
            if (titles) t_sublist = titles->get_shared_images(s[0],s[1]);
            sublist._display(disp,0,titles?&t_sublist:0,false,axis,align,XYZ,exit_on_anykey,
                             orig + s[0],false,is_exit);
          }
          disp.set_title("%s",dtitle.data());