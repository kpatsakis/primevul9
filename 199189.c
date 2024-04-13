    //! Convert pixel values from RGB to HSV color spaces.
    CImg<T>& RGBtoHSV() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoHSV(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=256))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          m = cimg::min(R,G,B),
          M = cimg::max(R,G,B);
        Tfloat H = 0, S = 0;
        if (M!=m) {
          const Tfloat
            f = R==m?G - B:G==m?B - R:R - G,
            i = R==m?3:G==m?5:1;
          H = i - f/(M - m);
          if (H>=6) H-=6;
          H*=60;
          S = (M - m)/M;
        }
        p1[N] = (T)cimg::cut(H,0,360);
        p2[N] = (T)cimg::cut(S,0,1);
        p3[N] = (T)cimg::cut(M/255,0,1);
      }
      return *this;