    //! Compute the tangent of each pixel value \newinstance.
    CImg<Tfloat> get_tan() const {
      return CImg<Tfloat>(*this,false).tan();