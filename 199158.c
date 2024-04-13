    //! Compute the arcsine of each pixel value \newinstance.
    CImg<Tfloat> get_asin() const {
      return CImg<Tfloat>(*this,false).asin();