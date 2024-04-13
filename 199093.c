    **/
    CImg<T>& sqr() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=524288))
      cimg_rof(*this,ptrd,T) { const T val = *ptrd; *ptrd = (T)(val*val); };
      return *this;