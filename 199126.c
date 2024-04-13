    // Conversion functions to get more precision when trying to store unsigned ints values as floats.
    inline unsigned int float2uint(const float f) {
      int tmp = 0;
      std::memcpy(&tmp,&f,sizeof(float));
      if (tmp>=0) return (unsigned int)f;
      unsigned int u;
      // use memcpy instead of assignment to avoid undesired optimizations by C++-compiler.
      std::memcpy(&u,&f,sizeof(float));
      return ((u)<<1)>>1; // set sign bit to 0.