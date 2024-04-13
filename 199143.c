    template<typename T>
    inline double cbrt(const T& x) {
#if cimg_use_cpp11==1
      return std::cbrt(x);
#else
      return x>=0?std::pow((double)x,1.0/3):-std::pow(-(double)x,1.0/3);
#endif