    **/
    inline double rand(const double val_min, const double val_max) {
      const double val = (double)std::rand()/RAND_MAX;
      return val_min + (val_max - val_min)*val;