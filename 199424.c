    //! Compute the sine of each pixel value \newinstance.
    CImg<Tfloat> get_sin() const {
      return CImg<Tfloat>(*this,false).sin();