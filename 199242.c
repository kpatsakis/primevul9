    //! Resize 3d object to unit size \newinstance.
    CImg<Tfloat> get_resize_object3d() const {
      return CImg<Tfloat>(*this,false).resize_object3d();