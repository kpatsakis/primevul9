    //! Compute the sinc of each pixel value \newinstance.
    CImg<Tfloat> get_sinc() const {
      return CImg<Tfloat>(*this,false).sinc();