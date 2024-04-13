    //! Convert 3d object into a CImg3d representation \overloading.
    CImg<T>& object3dtoCImg3d(const bool full_check=true) {
      return get_object3dtoCImg3d(full_check).move_to(*this);