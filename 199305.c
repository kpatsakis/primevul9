    **/
    inline unsigned int prand(const double z) {
      if (z<=1.0e-10) return 0;
      if (z>100) return (unsigned int)((std::sqrt(z) * cimg::grand()) + z);
      unsigned int k = 0;
      const double y = std::exp(-z);
      for (double s = 1.0; s>=y; ++k) s*=cimg::rand();
      return k - 1;