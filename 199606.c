    //! Return <tt>1 + log_10(x)</tt> of a value \c x.
    inline int xln(const int x) {
      return x>0?(int)(1 + std::log10((double)x)):1;