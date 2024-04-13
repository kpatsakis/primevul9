    **/
    CImg<T>& rol(const unsigned int n=1) {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=32768))
      cimg_rof(*this,ptrd,T) *ptrd = (T)cimg::rol(*ptrd,n);
      return *this;