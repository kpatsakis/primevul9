
    T _cubic_cut_atXY(const float fx, const float fy, const int z, const int c) const {
      return cimg::type<T>::cut(_cubic_atXY(fx,fy,z,c));