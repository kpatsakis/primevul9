    //! Compute the cosine of each pixel value \newinstance.
    CImg<Tfloat> get_cos() const {
      return CImg<Tfloat>(*this,false).cos();