    //! Compute the sign of each pixel value \newinstance.
    CImg<Tfloat> get_sign() const {
      return CImg<Tfloat>(*this,false).sign();