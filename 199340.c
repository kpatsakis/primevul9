
    inline double rand(const double val_min, const double val_max) {
      const double val = cimg::_rand()/16777215.;
      return val_min + (val_max - val_min)*val;