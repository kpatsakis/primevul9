    //! Compute the hyperbolic cosine of each pixel value \newinstance.
    CImg<Tfloat> get_cosh() const {
      return CImg<Tfloat>(*this,false).cosh();