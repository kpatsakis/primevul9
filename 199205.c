    //! Load image from an ascii file \inplace.
    static CImg<T> get_load_ascii(const char *const filename) {
      return CImg<T>().load_ascii(filename);