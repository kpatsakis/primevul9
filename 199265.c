    //! Access to pixel value with Dirichlet boundary conditions \const.
    T atNXYZC(const int pos, const int x, const int y, const int z, const int c, const T& out_value) const {
      return (pos<0 || pos>=(int)_width)?out_value:_data[pos].atXYZC(x,y,z,c,out_value);