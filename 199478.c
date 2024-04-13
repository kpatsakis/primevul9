    /**
       \param[out] max_val Value of the maximum value found.
    **/
    template<typename t>
    T& min_max(t& max_val) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "min_max(): Empty instance.",
                                    cimglist_instance);
      T *ptr_min = _data->_data;
      T min_value = *ptr_min, max_value = min_value;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) {
          const T val = *ptrs;
          if (val<min_value) { min_value = val; ptr_min = ptrs; }
          if (val>max_value) max_value = val;
        }