    //! Compute the exponential of each pixel value \newinstance.
    CImg<Tfloat> get_exp() const {
      return CImg<Tfloat>(*this,false).exp();