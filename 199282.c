    //! Convert pixel values from CMYK to CMY color spaces \newinstance.
    CImg<Tfloat> get_CMYKtoCMY() const {
      if (_spectrum!=4)
        throw CImgInstanceException(_cimg_instance
                                    "CMYKtoCMY(): Instance is not a CMYK image.",
                                    cimg_instance);

      CImg<Tfloat> res(_width,_height,_depth,3);
      const T *ps1 = data(0,0,0,0), *ps2 = data(0,0,0,1), *ps3 = data(0,0,0,2), *ps4 = data(0,0,0,3);
      Tfloat *pd1 = res.data(0,0,0,0), *pd2 = res.data(0,0,0,1), *pd3 = res.data(0,0,0,2);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=1024))
      for (ulongT N = 0; N<whd; ++N) {
        const Tfloat
	  C = (Tfloat)ps1[N],
	  M = (Tfloat)ps2[N],
	  Y = (Tfloat)ps3[N],
	  K = (Tfloat)ps4[N],
	  K1 = 1 - K/255,
          nC = C*K1 + K,
          nM = M*K1 + K,
          nY = Y*K1 + K;
        pd1[N] = (Tfloat)cimg::cut(nC,0,255),
        pd2[N] = (Tfloat)cimg::cut(nM,0,255),
        pd3[N] = (Tfloat)cimg::cut(nY,0,255);
      }
      return res;