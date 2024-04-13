    //! Pointwise max operator between instance image and a value \newinstance.
    CImg<T> get_max(const T& val) const {
      return (+*this).max(val);