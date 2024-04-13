    //! Convert pixel values from RGB to CMY color spaces.
    CImg<T>& RGBtoCMY() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoCMY(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=2048))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          C = 255 - R,
          M = 255 - G,
          Y = 255 - B;
        p1[N] = (T)cimg::cut(C,0,255),
        p2[N] = (T)cimg::cut(M,0,255),
        p3[N] = (T)cimg::cut(Y,0,255);
      }
      return *this;