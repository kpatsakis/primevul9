    **/
    CImg<T>& cosh() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=2048))
      cimg_rof(*this,ptrd,T) *ptrd = (T)std::cosh((double)*ptrd);
      return *this;