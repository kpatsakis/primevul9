    /**
       \param[out] min_val Value of the minimum value found.
    **/
    template<typename t>
    T& max_min(t& min_val) {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "max_min(): Empty instance.",
                                    cimglist_instance);
      T *ptr_max = _data->_data;
      T min_value = *ptr_max, max_value = min_value;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) {
          const T val = *ptrs;
          if (val>max_value) { max_value = val; ptr_max = ptrs; }
          if (val<min_value) min_value = val;
        }