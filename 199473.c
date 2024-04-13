    //! Convert 3d object into a CImg3d representation \overloading.
    CImg<floatT> get_object3dtoCImg3d(const bool full_check=true) const {
      CImgList<T> opacities, colors;
      CImgList<uintT> primitives(width(),1,1,1,1);
      cimglist_for(primitives,p) primitives(p,0) = p;
      return get_object3dtoCImg3d(primitives,colors,opacities,full_check);