    //! Convert pixel values from RGB to YCbCr color spaces.
    CImg<T>& YCbCrtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "YCbCrtoRGB(): Instance is not a YCbCr image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=512))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          Y = (Tfloat)p1[N] - 16,
          Cb = (Tfloat)p2[N] - 128,
          Cr = (Tfloat)p3[N] - 128,
          R = (298*Y + 409*Cr + 128)/256,
          G = (298*Y - 100*Cb - 208*Cr + 128)/256,
          B = (298*Y + 516*Cb + 128)/256;
        p1[N] = (T)cimg::cut(R,0,255),
        p2[N] = (T)cimg::cut(G,0,255),
        p3[N] = (T)cimg::cut(B,0,255);
      }
      return *this;