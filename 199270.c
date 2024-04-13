
    T _cubic_cut_atX(const float fx, const int y, const int z, const int c) const {
      return cimg::type<T>::cut(_cubic_atX(fx,y,z,c));