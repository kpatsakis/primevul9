     **/
    CImg<T> get_matrix_at(const unsigned int x=0, const unsigned int y=0, const unsigned int z=0) const {
      const int n = (int)std::sqrt((double)_spectrum);
      const T *ptrs = data(x,y,z,0);
      const ulongT whd = (ulongT)_width*_height*_depth;
      CImg<T> res(n,n);
      T *ptrd = res._data;
      cimg_forC(*this,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
      return res;