    //! Shift 3d object's vertices, so that it becomes centered \newinstance.
    CImg<Tfloat> get_shift_object3d() const {
      return CImg<Tfloat>(*this,false).shift_object3d();