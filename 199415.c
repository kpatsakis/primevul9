    //! Load image from an ascii file \overloading.
    CImg<T>& load_ascii(std::FILE *const file) {
      return _load_ascii(file,0);