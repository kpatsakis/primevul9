    //! Shift 3d object's vertices \newinstance.
    CImg<Tfloat> get_shift_object3d(const float tx, const float ty=0, const float tz=0) const {
      return CImg<Tfloat>(*this,false).shift_object3d(tx,ty,tz);