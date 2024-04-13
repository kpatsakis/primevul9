    //! Compute the square value of each pixel value \newinstance.
    CImg<Tfloat> get_sqr() const {
      return CImg<Tfloat>(*this,false).sqr();