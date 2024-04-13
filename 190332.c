  void incrementRecursionDepth() {
    if (recursion_limit_ < ++recursion_depth_) {
      throw TProtocolException(TProtocolException::DEPTH_LIMIT);
    }
  }