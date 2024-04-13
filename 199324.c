    //! Compute the square root of each pixel value \newinstance.
    CImg<Tfloat> get_sqrt() const {
      return CImg<Tfloat>(*this,false).sqrt();