    //! Access to pixel value with Dirichlet boundary conditions for the coordinate (\c pos) \const.
    T atN(const int pos, const int x, const int y, const int z, const int c, const T& out_value) const {
      return (pos<0 || pos>=(int)_width)?out_value:(*this)(pos,x,y,z,c);