  static double cap_channel(double c) {
    if      (c > range) return range;
    else if (c < 0)     return 0;
    else                return c;
  }