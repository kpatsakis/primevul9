    **/
    T cubic_cut_atXY(const float fx, const float fy, const int z, const int c, const T& out_value) const {
      return cimg::type<T>::cut(cubic_atXY(fx,fy,z,c,out_value));