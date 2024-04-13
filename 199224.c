    **/
    T cubic_cut_atX(const float fx, const int y, const int z, const int c, const T& out_value) const {
      return cimg::type<T>::cut(cubic_atX(fx,y,z,c,out_value));