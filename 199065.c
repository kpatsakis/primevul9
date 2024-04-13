    template<typename t, typename tc>
    CImg<T>& draw_gaussian(const float xc, const float yc, const CImg<t>& tensor,
                           const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (tensor._width!=2 || tensor._height!=2 || tensor._depth!=1 || tensor._spectrum!=1)
        throw CImgArgumentException(_cimg_instance
                                    "draw_gaussian(): Specified tensor (%u,%u,%u,%u,%p) is not a 2x2 matrix.",
                                    cimg_instance,
                                    tensor._width,tensor._height,tensor._depth,tensor._spectrum,tensor._data);
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_gaussian(): Specified color is (null).",
                                    cimg_instance);
      typedef typename CImg<t>::Tfloat tfloat;
      const CImg<tfloat> invT = tensor.get_invert(), invT2 = (invT*invT)/(-2.0);
      const tfloat a = invT2(0,0), b = 2*invT2(1,0), c = invT2(1,1);
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
      const ulongT whd = (ulongT)_width*_height*_depth;
      const tc *col = color;
      float dy = -yc;
      cimg_forY(*this,y) {
        float dx = -xc;
        cimg_forX(*this,x) {
          const float val = (float)std::exp(a*dx*dx + b*dx*dy + c*dy*dy);
          T *ptrd = data(x,y,0,0);
          if (opacity>=1) cimg_forC(*this,c) { *ptrd = (T)(val*(*col++)); ptrd+=whd; }
          else cimg_forC(*this,c) { *ptrd = (T)(nopacity*val*(*col++) + *ptrd*copacity); ptrd+=whd; }
          col-=_spectrum;
          ++dx;
        }
        ++dy;
      }
      return *this;