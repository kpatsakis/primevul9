    **/
    CImg<T>& round(const double y=1, const int rounding_type=0) {
      if (y>0)
        cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=8192))
        cimg_rof(*this,ptrd,T) *ptrd = cimg::round(*ptrd,y,rounding_type);
      return *this;