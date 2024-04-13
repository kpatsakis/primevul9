    //! Convert pixel values from CMY to CMYK color spaces \newinstance.
    CImg<Tuchar> get_CMYtoCMYK() const {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "CMYtoCMYK(): Instance is not a CMY image.",
                                    cimg_instance);

      CImg<Tfloat> res(_width,_height,_depth,4);
      const T *ps1 = data(0,0,0,0), *ps2 = data(0,0,0,1), *ps3 = data(0,0,0,2);
      Tfloat *pd1 = res.data(0,0,0,0), *pd2 = res.data(0,0,0,1), *pd3 = res.data(0,0,0,2), *pd4 = res.data(0,0,0,3);
      const ulongT whd = (ulongT)_width*_height*_depth;
      cimg_pragma_openmp(parallel for cimg_openmp_if(whd>=1024))
      for (ulongT N = 0; N<whd; ++N) {
        Tfloat
	  C = (Tfloat)ps1[N],
	  M = (Tfloat)ps2[N],
	  Y = (Tfloat)ps3[N],
	  K = cimg::min(C,M,Y);
	if (K>=255) C = M = Y = 0;
	else { const Tfloat K1 = 255 - K; C = 255*(C - K)/K1; M = 255*(M - K)/K1; Y = 255*(Y - K)/K1; }
        pd1[N] = (Tfloat)cimg::cut(C,0,255),
        pd2[N] = (Tfloat)cimg::cut(M,0,255),
        pd3[N] = (Tfloat)cimg::cut(Y,0,255),
        pd4[N] = (Tfloat)cimg::cut(K,0,255);
      }
      return res;