    //! Convert pixel values from RGB to YUV color spaces.
    CImg<T>& RGBtoYUV() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoYUV(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=16384))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N]/255,
          G = (Tfloat)p2[N]/255,
          B = (Tfloat)p3[N]/255,
          Y = 0.299f*R + 0.587f*G + 0.114f*B;
        p1[N] = (T)Y;
        p2[N] = (T)(0.492f*(B - Y));
        p3[N] = (T)(0.877*(R - Y));
      }
      return *this;