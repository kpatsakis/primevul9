    //! Compute the absolute value of each pixel value \newinstance.
    CImg<Tfloat> get_abs() const {
      return CImg<Tfloat>(*this,false).abs();