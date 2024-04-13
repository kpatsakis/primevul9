    **/
    CImg<T>& log2() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=4096))
      cimg_rof(*this,ptrd,T) *ptrd = (T)cimg::log2((double)*ptrd);
      return *this;