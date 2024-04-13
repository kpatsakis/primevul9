    //! Compute the arctangent of each pixel value \newinstance.
    CImg<Tfloat> get_atan() const {
      return CImg<Tfloat>(*this,false).atan();