    //! Compute the base-10 logarithm of each pixel value \newinstance.
    CImg<Tfloat> get_log10() const {
      return CImg<Tfloat>(*this,false).log10();