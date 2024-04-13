    //! Compute the arccosine of each pixel value \newinstance.
    CImg<Tfloat> get_acos() const {
      return CImg<Tfloat>(*this,false).acos();