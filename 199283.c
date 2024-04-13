    template<typename tc>
    CImg<T>& draw_gaussian(const float xc, const float sigma,
                           const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_gaussian(): Specified color is (null).",
                                    cimg_instance);
      const float sigma2 = 2*sigma*sigma, nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
      const ulongT whd = (ulongT)_width*_height*_depth;
      const tc *col = color;
      cimg_forX(*this,x) {
        const float dx = (x - xc), val = (float)std::exp(-dx*dx/sigma2);
        T *ptrd = data(x,0,0,0);
        if (opacity>=1) cimg_forC(*this,c) { *ptrd = (T)(val*(*col++)); ptrd+=whd; }
        else cimg_forC(*this,c) { *ptrd = (T)(nopacity*val*(*col++) + *ptrd*copacity); ptrd+=whd; }
        col-=_spectrum;
      }
      return *this;