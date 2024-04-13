    //! Compute the hyperbolic sine of each pixel value \newinstance.
    CImg<Tfloat> get_sinh() const {
      return CImg<Tfloat>(*this,false).sinh();