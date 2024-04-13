     **/
    CImg<T>& max(const T& val) {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=65536))
      cimg_rof(*this,ptrd,T) *ptrd = std::max(*ptrd,val);
      return *this;