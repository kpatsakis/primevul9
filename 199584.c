    //! Compute the hyperbolic tangent of each pixel value \newinstance.
    CImg<Tfloat> get_tanh() const {
      return CImg<Tfloat>(*this,false).tanh();