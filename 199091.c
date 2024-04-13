    //! Compute the base-10 logarithm of each pixel value \newinstance.
    CImg<Tfloat> get_log2() const {
      return CImg<Tfloat>(*this,false).log2();