  TRecursionTracker(TProtocol &prot) : prot_(prot) {
    prot_.incrementRecursionDepth();
  }