    //! Convert pixel values from HSV to RGB color spaces.
    CImg<T>& HSVtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "HSVtoRGB(): Instance is not a HSV image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=256))
      for (ulongT N = 0; N<whd; ++N) {
        Tfloat
          H = cimg::mod((Tfloat)p1[N],(Tfloat)360),
          S = (Tfloat)p2[N],
          V = (Tfloat)p3[N],
          R = 0, G = 0, B = 0;
        if (H==0 && S==0) R = G = B = V;
        else {
          H/=60;
          const int i = (int)std::floor(H);
          const Tfloat
            f = (i&1)?H - i:1 - H + i,
            m = V*(1 - S),
            n = V*(1 - S*f);
          switch (i) {
          case 6 :
          case 0 : R = V; G = n; B = m; break;
          case 1 : R = n; G = V; B = m; break;
          case 2 : R = m; G = V; B = n; break;
          case 3 : R = m; G = n; B = V; break;
          case 4 : R = n; G = m; B = V; break;
          case 5 : R = V; G = m; B = n; break;
          }
        }
        p1[N] = (T)cimg::cut(R*255,0,255);
        p2[N] = (T)cimg::cut(G*255,0,255);
        p3[N] = (T)cimg::cut(B*255,0,255);
      }
      return *this;