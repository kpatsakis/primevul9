    **/
    CImg<T>& pow(const double p) {
      if (is_empty()) return *this;
      if (p==-4) {
        cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=32768))
        cimg_rof(*this,ptrd,T) { const T val = *ptrd; *ptrd = (T)(1.0/(val*val*val*val)); }
        return *this;
      }
      if (p==-3) {
        cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=32768))
        cimg_rof(*this,ptrd,T) { const T val = *ptrd; *ptrd = (T)(1.0/(val*val*val)); }
        return *this;
      }
      if (p==-2) {
        cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=32768))
        cimg_rof(*this,ptrd,T) { const T val = *ptrd; *ptrd = (T)(1.0/(val*val)); }
        return *this;
      }
      if (p==-1) {
        cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=32768))
        cimg_rof(*this,ptrd,T) { const T val = *ptrd; *ptrd = (T)(1.0/val); }
        return *this;
      }
      if (p==-0.5) {
        cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=8192))
        cimg_rof(*this,ptrd,T) { const T val = *ptrd; *ptrd = (T)(1/std::sqrt((double)val)); }
        return *this;
      }
      if (p==0) return fill((T)1);
      if (p==0.25) return sqrt().sqrt();
      if (p==0.5) return sqrt();
      if (p==1) return *this;
      if (p==2) return sqr();
      if (p==3) {
        cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=262144))
        cimg_rof(*this,ptrd,T) { const T val = *ptrd; *ptrd = val*val*val; }
        return *this;
      }
      if (p==4) {
        cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=131072))
        cimg_rof(*this,ptrd,T) { const T val = *ptrd; *ptrd = val*val*val*val; }
        return *this;
      }
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=1024))
      cimg_rof(*this,ptrd,T) *ptrd = (T)std::pow((double)*ptrd,p);
      return *this;