    **/
    CImg<T>& RGBtoXYZ(const bool use_D65=true) {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoXYZ(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=2048))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N]/255,
          G = (Tfloat)p2[N]/255,
          B = (Tfloat)p3[N]/255;
        if (use_D65) { // D65
          p1[N] = (T)(0.4124564*R + 0.3575761*G + 0.1804375*B);
          p2[N] = (T)(0.2126729*R + 0.7151522*G + 0.0721750*B);
          p3[N] = (T)(0.0193339*R + 0.1191920*G + 0.9503041*B);
        } else { // D50
          p1[N] = (T)(0.43603516*R + 0.38511658*G + 0.14305115*B);
          p2[N] = (T)(0.22248840*R + 0.71690369*G + 0.06060791*B);
          p3[N] = (T)(0.01391602*R + 0.09706116*G + 0.71392822*B);
        }
      }
      return *this;