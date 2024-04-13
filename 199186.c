    template<typename T>
    inline T sign(const T& x) {
      return (T)(x<0?-1:x>0);