
    inline float uint2float(const unsigned int u) {
      if (u<(1U<<19)) return (float)u;  // Consider safe storage of unsigned int as floats until 19bits (i.e 524287).
      float f;
      const unsigned int v = u|(1U<<(8*sizeof(unsigned int)-1)); // set sign bit to 1.
      // use memcpy instead of simple assignment to avoid undesired optimizations by C++-compiler.
      std::memcpy(&f,&v,sizeof(float));
      return f;