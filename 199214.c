    **/
    CImg<T>& sinh() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=2048))
      cimg_rof(*this,ptrd,T) *ptrd = (T)std::sinh((double)*ptrd);
      return *this;