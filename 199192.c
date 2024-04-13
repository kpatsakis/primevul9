      }
      static bool is_nan(const double val) {
#ifdef isnan
        return (bool)isnan(val);
#else
        return !(val==val);
#endif