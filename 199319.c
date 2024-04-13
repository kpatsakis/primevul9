    template<typename t>
    CImg<T>& operator<<=(const t value) {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=65536))
      cimg_rof(*this,ptrd,T) *ptrd = (T)(((longT)*ptrd) << (int)value);
      return *this;