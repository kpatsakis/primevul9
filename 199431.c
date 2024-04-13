    **/
    CImg<T>& operator--() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=524288))
      cimg_rof(*this,ptrd,T) *ptrd = *ptrd - (T)1;
      return *this;