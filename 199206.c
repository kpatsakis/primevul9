    //! Pointwise min operator between instance image and a value \newinstance.
    CImg<T> get_min(const T& val) const {
      return (+*this).min(val);