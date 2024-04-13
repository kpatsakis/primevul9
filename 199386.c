    //! Convert pixel values from HSL to RGB color spaces.
    CImg<T>& HSLtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "HSLtoRGB(): Instance is not a HSL image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=256))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          H = cimg::mod((Tfloat)p1[N],(Tfloat)360),
          S = (Tfloat)p2[N],
          L = (Tfloat)p3[N],
          q = 2*L<1?L*(1 + S):L + S - L*S,
          p = 2*L - q,
          h = H/360,
          tr = h + (Tfloat)1/3,
          tg = h,
          tb = h - (Tfloat)1/3,
          ntr = tr<0?tr + 1:tr>1?tr - 1:(Tfloat)tr,
          ntg = tg<0?tg + 1:tg>1?tg - 1:(Tfloat)tg,
          ntb = tb<0?tb + 1:tb>1?tb - 1:(Tfloat)tb,
          R = 6*ntr<1?p + (q - p)*6*ntr:2*ntr<1?q:3*ntr<2?p + (q - p)*6*(2.0f/3 - ntr):p,
          G = 6*ntg<1?p + (q - p)*6*ntg:2*ntg<1?q:3*ntg<2?p + (q - p)*6*(2.0f/3 - ntg):p,
          B = 6*ntb<1?p + (q - p)*6*ntb:2*ntb<1?q:3*ntb<2?p + (q - p)*6*(2.0f/3 - ntb):p;
        p1[N] = (T)cimg::cut(255*R,0,255);
        p2[N] = (T)cimg::cut(255*G,0,255);
        p3[N] = (T)cimg::cut(255*B,0,255);
      }
      return *this;