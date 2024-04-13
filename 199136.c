      }
      static bool is_nan(const float val) {
#ifdef isnan
        return (bool)isnan(val);
#else
        return !(val==val);
#endif