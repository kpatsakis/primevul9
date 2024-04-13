    //! Convert pixel values from HSI to RGB color spaces.
    CImg<T>& HSItoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "HSItoRGB(): Instance is not a HSI image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=256))
      for (ulongT N = 0; N<whd; ++N) {
        Tfloat
          H = cimg::mod((Tfloat)p1[N],(Tfloat)360),
          S = (Tfloat)p2[N],
          I = (Tfloat)p3[N],
          a = I*(1 - S),
          R = 0, G = 0, B = 0;
        if (H<120) {
          B = a;
          R = (Tfloat)(I*(1 + S*std::cos(H*cimg::PI/180)/std::cos((60 - H)*cimg::PI/180)));
          G = 3*I - (R + B);
        } else if (H<240) {
          H-=120;
          R = a;
          G = (Tfloat)(I*(1 + S*std::cos(H*cimg::PI/180)/std::cos((60 - H)*cimg::PI/180)));
          B = 3*I - (R + G);
        } else {
          H-=240;
          G = a;
          B = (Tfloat)(I*(1 + S*std::cos(H*cimg::PI/180)/std::cos((60 - H)*cimg::PI/180)));
          R = 3*I - (G + B);
        }
        p1[N] = (T)cimg::cut(R*255,0,255);
        p2[N] = (T)cimg::cut(G*255,0,255);
        p3[N] = (T)cimg::cut(B*255,0,255);
      }
      return *this;