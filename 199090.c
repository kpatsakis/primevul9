    //! Autocrop image region, regarding the specified background color \newinstance.
    template<typename t> CImg<T> get_autocrop(const CImg<t>& color, const char *const axes="zyx") const {
      return get_autocrop(color._data,axes);