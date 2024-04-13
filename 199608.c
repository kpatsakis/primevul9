    //! Convert pixel values from RGB to YCbCr color spaces.
    CImg<T>& RGBtoYCbCr() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoYCbCr(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=512))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          Y = (66*R + 129*G + 25*B + 128)/256 + 16,
          Cb = (-38*R - 74*G + 112*B + 128)/256 + 128,
          Cr = (112*R - 94*G - 18*B + 128)/256 + 128;
        p1[N] = (T)cimg::cut(Y,0,255),
        p2[N] = (T)cimg::cut(Cb,0,255),
        p3[N] = (T)cimg::cut(Cr,0,255);
      }
      return *this;