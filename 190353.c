  ~TRecursionTracker() {
    prot_.decrementRecursionDepth();
  }