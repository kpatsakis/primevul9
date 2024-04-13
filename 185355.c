  template <Type type> static void finalize() {
    ASSERT(!mutableFinalized<type>());
    mutableFinalized<type>() = true;
  }