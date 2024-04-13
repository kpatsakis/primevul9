    //! Compute the logarithm of each pixel value \newinstance.
    CImg<Tfloat> get_log() const {
      return CImg<Tfloat>(*this,false).log();