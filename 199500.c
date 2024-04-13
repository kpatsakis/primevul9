    //! Construct a 2d representation of a 3d image, with XY,XZ and YZ views \inplace.
    CImg<T>& projections2d(const unsigned int x0, const unsigned int y0, const unsigned int z0) {
      if (_depth<2) return *this;
      return get_projections2d(x0,y0,z0).move_to(*this);