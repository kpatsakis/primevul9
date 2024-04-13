    //! Convert pixel values from XYZ to Lab color spaces.
    CImg<T>& XYZtoLab(const bool use_D65=true) {
#define _cimg_Labf(x) (24389*(x)>216?cimg::cbrt(x):(24389*(x)/27 + 16)/116)

      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "XYZtoLab(): Instance is not a XYZ image.",
                                    cimg_instance);
      const CImg<Tfloat> white = CImg<Tfloat>(1,1,1,3,255).RGBtoXYZ(use_D65);
      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=128))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          X = (Tfloat)(p1[N]/white[0]),
          Y = (Tfloat)(p2[N]/white[1]),
          Z = (Tfloat)(p3[N]/white[2]),
          fX = (Tfloat)_cimg_Labf(X),
          fY = (Tfloat)_cimg_Labf(Y),
          fZ = (Tfloat)_cimg_Labf(Z);
        p1[N] = (T)cimg::cut(116*fY - 16,0,100);
        p2[N] = (T)(500*(fX - fY));
        p3[N] = (T)(200*(fY - fZ));
      }
      return *this;