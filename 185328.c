  static std::unique_ptr<RequestHeaderMapImpl> create() {
    return std::unique_ptr<RequestHeaderMapImpl>(new (inlineHeadersSize()) RequestHeaderMapImpl());
  }