    **/
    CImg<T>& distance(const T& value, const unsigned int metric=2) {
      if (is_empty()) return *this;
      if (cimg::type<Tint>::string()!=cimg::type<T>::string()) // For datatype < int.
        return CImg<Tint>(*this,false).distance((Tint)value,metric).
          cut((Tint)cimg::type<T>::min(),(Tint)cimg::type<T>::max()).move_to(*this);
      bool is_value = false;
      cimg_for(*this,ptr,T) *ptr = *ptr==value?is_value=true,(T)0:(T)std::max(0,99999999); // (avoid VC++ warning)
      if (!is_value) return fill(cimg::type<T>::max());
      switch (metric) {
      case 0 : return _distance_core(_distance_sep_cdt,_distance_dist_cdt);          // Chebyshev.
      case 1 : return _distance_core(_distance_sep_mdt,_distance_dist_mdt);          // Manhattan.
      case 3 : return _distance_core(_distance_sep_edt,_distance_dist_edt);          // Squared Euclidean.
      default : return _distance_core(_distance_sep_edt,_distance_dist_edt).sqrt();  // Euclidean.
      }
      return *this;