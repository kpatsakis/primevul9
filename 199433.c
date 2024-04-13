    **/
    T cubic_cut_atXYZ(const float fx, const float fy, const float fz, const int c, const T& out_value) const {
      return cimg::type<T>::cut(cubic_atXYZ(fx,fy,fz,c,out_value));