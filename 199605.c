    inline unsigned int& _openmp_mode(const unsigned int value, const bool is_set) {
      static unsigned int mode = 2;
      if (is_set)  { cimg::mutex(0); mode = value<2?value:2; cimg::mutex(0,0); }
      return mode;
    }