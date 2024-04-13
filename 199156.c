    //! Resize 3d object \newinstance.
    CImg<Tfloat> get_resize_object3d(const float sx, const float sy=-100, const float sz=-100) const {
      return CImg<Tfloat>(*this,false).resize_object3d(sx,sy,sz);