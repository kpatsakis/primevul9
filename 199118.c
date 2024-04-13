    //! Return base-2 logarithm of a value.
    inline double log2(const double x) {
      const double base = std::log(2.0);
      return std::log(x)/base;