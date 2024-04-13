    //! Convert pixel values from RGB to sRGB color spaces.
    CImg<T>& RGBtosRGB() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=32))
      cimg_rof(*this,ptr,T) {
        const Tfloat
          val = (Tfloat)*ptr/255,
          sval = (Tfloat)(val<=0.0031308f?val*12.92f:1.055f*std::pow(val,0.416667f) - 0.055f);
        *ptr = (T)cimg::cut(sval*255,0,255);
      }
      return *this;