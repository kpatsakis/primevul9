      return *ptr_max;
    }

    //! Return a reference to the maximum pixel value of the instance list \const.
    const T& max() const {
      if (is_empty())
        throw CImgInstanceException(_cimglist_instance
                                    "max(): Empty instance.",
                                    cimglist_instance);
      const T *ptr_max = _data->_data;
      T max_value = *ptr_max;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];