    template<typename tc>
    CImg<T>& draw_point(const int x0, const int y0, const int z0,
                        const tc *const color, const float opacity=1) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_point(): Specified color is (null).",
                                    cimg_instance);
      if (x0>=0 && y0>=0 && z0>=0 && x0<width() && y0<height() && z0<depth()) {
        const ulongT whd = (ulongT)_width*_height*_depth;
        const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
        T *ptrd = data(x0,y0,z0,0);
        const tc *col = color;
        if (opacity>=1) cimg_forC(*this,c) { *ptrd = (T)*(col++); ptrd+=whd; }
        else cimg_forC(*this,c) { *ptrd = (T)(*(col++)*nopacity + *ptrd*copacity); ptrd+=whd; }
      }
      return *this;