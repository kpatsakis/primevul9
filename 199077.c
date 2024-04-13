    template<typename T>
    inline T round(const T& x, const double y, const int rounding_type=0) {
      if (y<=0) return x;
      if (y==1) switch (rounding_type) {
        case 0 : return round(x);
        case 1 : return (T)std::ceil((_cimg_Tfloat)x);
        default : return (T)std::floor((_cimg_Tfloat)x);
        }
      const double sx = (double)x/y, floor = std::floor(sx), delta =  sx - floor;
      return (T)(y*(rounding_type<0?floor:rounding_type>0?std::ceil(sx):delta<0.5?floor:std::ceil(sx)));