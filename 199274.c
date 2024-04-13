    //! Loadimage from an ascii file \newinstance.
    static CImg<T> get_load_ascii(std::FILE *const file) {
      return CImg<T>().load_ascii(file);