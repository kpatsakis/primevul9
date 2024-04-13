    **/
    CImgList<Tfloat> get_FFT(const char axis, const bool is_invert=false) const {
      CImgList<Tfloat> res(*this,CImg<Tfloat>());
      CImg<Tfloat>::FFT(res[0],res[1],axis,is_invert);
      return res;