    **/
    CImg<T>& XYZtoRGB(const bool use_D65=true) {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "XYZtoRGB(): Instance is not a XYZ image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=2048))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          X = (Tfloat)p1[N]*255,
          Y = (Tfloat)p2[N]*255,
          Z = (Tfloat)p3[N]*255;
        if (use_D65) {
          p1[N] = (T)cimg::cut(3.2404542*X - 1.5371385*Y - 0.4985314*Z,0,255);
          p2[N] = (T)cimg::cut(-0.9692660*X + 1.8760108*Y + 0.0415560*Z,0,255);
          p3[N] = (T)cimg::cut(0.0556434*X - 0.2040259*Y + 1.0572252*Z,0,255);
        } else {
          p1[N] = (T)cimg::cut(3.134274799724*X  - 1.617275708956*Y - 0.490724283042*Z,0,255);
          p2[N] = (T)cimg::cut(-0.978795575994*X + 1.916161689117*Y + 0.033453331711*Z,0,255);
          p3[N] = (T)cimg::cut(0.071976988401*X - 0.228984974402*Y + 1.405718224383*Z,0,255);
        }
      }
      return *this;