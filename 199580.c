    // Use a custom RNG.
    inline unsigned int _rand(const unsigned int seed=0, const bool set_seed=false) {
      static cimg_ulong next = 0xB16B00B5;
      cimg::mutex(4);
      if (set_seed) next = (cimg_ulong)seed;
      else next = next*1103515245 + 12345U;
      cimg::mutex(4,0);
      return (unsigned int)(next&0xFFFFFFU);