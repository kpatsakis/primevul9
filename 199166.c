    inline unsigned int& _exception_mode(const unsigned int value, const bool is_set) {
      static unsigned int mode = cimg_verbosity;
      if (is_set) { cimg::mutex(0); mode = value<4?value:4; cimg::mutex(0,0); }
      return mode;
    }