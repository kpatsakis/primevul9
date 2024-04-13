
    //! Return a reference to the minimum pixel value of the instance list.
    /**
    **/
    T& min() {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "min(): Empty instance.",
                                    cimglist_instance);
      T *ptr_min = _data->_data;
      T min_value = *ptr_min;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];