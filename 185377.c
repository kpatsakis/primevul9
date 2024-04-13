  static size_t inlineHeadersSize() {
    return StaticLookupTable<Interface>::size() * sizeof(HeaderEntryImpl*);
  }