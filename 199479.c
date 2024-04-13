    **/
    CImg<T>& sqrt() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=8192))
      cimg_rof(*this,ptrd,T) *ptrd = (T)std::sqrt((double)*ptrd);
      return *this;