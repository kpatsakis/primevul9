    //! Convert pixel values from XYZ to xyY color spaces.
    CImg<T>& XYZtoxyY() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "XYZtoxyY(): Instance is not a XYZ image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=4096))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          X = (Tfloat)p1[N],
          Y = (Tfloat)p2[N],
          Z = (Tfloat)p3[N],
          sum = X + Y + Z,
          nsum = sum>0?sum:1;
        p1[N] = (T)(X/nsum);
        p2[N] = (T)(Y/nsum);
        p3[N] = (T)Y;
      }
      return *this;