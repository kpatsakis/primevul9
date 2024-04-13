    //! Autocrop image region, regarding the specified background color \overloading.
    template<typename t> CImg<T>& autocrop(const CImg<t>& color, const char *const axes="zyx") {
      return get_autocrop(color,axes).move_to(*this);