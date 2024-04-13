    //! Convert pixel values from Lab to XYZ color spaces.
    CImg<T>& LabtoXYZ(const bool use_D65=true) {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "LabtoXYZ(): Instance is not a Lab image.",
                                    cimg_instance);
      const CImg<Tfloat> white = CImg<Tfloat>(1,1,1,3,255).RGBtoXYZ(use_D65);
      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=128))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
          L = (Tfloat)p1[N],
          a = (Tfloat)p2[N],
          b = (Tfloat)p3[N],
          cY = (L + 16)/116,
          cZ = cY - b/200,
          cX = a/500 + cY,
          X = (Tfloat)(24389*cX>216?cX*cX*cX:(116*cX - 16)*27/24389),
          Y = (Tfloat)(27*L>216?cY*cY*cY:27*L/24389),
          Z = (Tfloat)(24389*cZ>216?cZ*cZ*cZ:(116*cZ - 16)*27/24389);
        p1[N] = (T)(X*white[0]);
        p2[N] = (T)(Y*white[1]);
        p3[N] = (T)(Z*white[2]);
      }
      return *this;