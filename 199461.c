    //! Convert pixel values from RGB to HSI color spaces.
    CImg<T>& RGBtoHSI() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoHSI(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=256))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          theta = (Tfloat)(std::acos(0.5f*((R - G) + (R - B))/
                                     std::sqrt(cimg::sqr(R - G) + (R - B)*(G - B)))*180/cimg::PI),
          m = cimg::min(R,G,B),
          sum = R + G + B;
        Tfloat H = 0, S = 0, I = 0;
        if (theta>0) H = B<=G?theta:360 - theta;
        if (sum>0) S = 1 - 3*m/sum;
        I = sum/(3*255);
        p1[N] = (T)cimg::cut(H,0,360);
        p2[N] = (T)cimg::cut(S,0,1);
        p3[N] = (T)cimg::cut(I,0,1);
      }
      return *this;