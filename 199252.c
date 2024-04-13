    template<typename tp, typename tc, typename to>
    CImg<floatT> get_object3dtoCImg3d(const CImgList<tp>& primitives,
                                      const CImgList<tc>& colors,
                                      const to& opacities,
                                      const bool full_check=true) const {
      CImg<charT> error_message(1024);
      if (!is_object3d(primitives,colors,opacities,full_check,error_message))
        throw CImgInstanceException(_cimg_instance
                                    "object3dtoCImg3d(): Invalid specified 3d object (%u,%u) (%s).",
                                    cimg_instance,_width,primitives._width,error_message.data());
      CImg<floatT> res(1,_size_object3dtoCImg3d(primitives,colors,opacities));
      float *ptrd = res._data;

      // Put magick number.
      *(ptrd++) = 'C' + 0.5f; *(ptrd++) = 'I' + 0.5f; *(ptrd++) = 'm' + 0.5f;
      *(ptrd++) = 'g' + 0.5f; *(ptrd++) = '3' + 0.5f; *(ptrd++) = 'd' + 0.5f;

      // Put number of vertices and primitives.
      *(ptrd++) = cimg::uint2float(_width);
      *(ptrd++) = cimg::uint2float(primitives._width);

      // Put vertex data.
      if (is_empty() || !primitives) return res;
      const T *ptrx = data(0,0), *ptry = data(0,1), *ptrz = data(0,2);
      cimg_forX(*this,p) {
        *(ptrd++) = (float)*(ptrx++);
        *(ptrd++) = (float)*(ptry++);
        *(ptrd++) = (float)*(ptrz++);
      }

      // Put primitive data.
      cimglist_for(primitives,p) {
        *(ptrd++) = (float)primitives[p].size();
        const tp *ptrp = primitives[p]._data;
        cimg_foroff(primitives[p],i) *(ptrd++) = cimg::uint2float((unsigned int)*(ptrp++));
      }

      // Put color/texture data.
      const unsigned int csiz = std::min(colors._width,primitives._width);
      for (int c = 0; c<(int)csiz; ++c) {
        const CImg<tc>& color = colors[c];
        const tc *ptrc = color._data;
        if (color.size()==3) { *(ptrd++) = (float)*(ptrc++); *(ptrd++) = (float)*(ptrc++); *(ptrd++) = (float)*ptrc; }
        else {
          *(ptrd++) = -128.0f;
          int shared_ind = -1;
          if (color.is_shared()) for (int i = 0; i<c; ++i) if (ptrc==colors[i]._data) { shared_ind = i; break; }
          if (shared_ind<0) {
            *(ptrd++) = (float)color._width;
            *(ptrd++) = (float)color._height;
            *(ptrd++) = (float)color._spectrum;
            cimg_foroff(color,l) *(ptrd++) = (float)*(ptrc++);
          } else {
            *(ptrd++) = (float)shared_ind;
            *(ptrd++) = 0;
            *(ptrd++) = 0;
          }
        }
      }
      const int csiz2 = primitives.width() - colors.width();
      for (int c = 0; c<csiz2; ++c) { *(ptrd++) = 200.0f; *(ptrd++) = 200.0f; *(ptrd++) = 200.0f; }

      // Put opacity data.
      ptrd = _object3dtoCImg3d(opacities,ptrd);
      const float *ptre = res.end();
      while (ptrd<ptre) *(ptrd++) = 1.0f;
      return res;