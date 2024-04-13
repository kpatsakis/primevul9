    **/
    CImg<T>& cut(const T& min_value, const T& max_value) {
      if (is_empty()) return *this;
      const T a = min_value<max_value?min_value:max_value, b = min_value<max_value?max_value:min_value;
      cimg_pragma_openmp(parallel for cimg_openmp_if(size()>=32768))
      cimg_rof(*this,ptrd,T) *ptrd = (*ptrd<a)?a:((*ptrd>b)?b:*ptrd);
      return *this;