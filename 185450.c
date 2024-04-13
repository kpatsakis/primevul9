  static std::unique_ptr<ResponseHeaderMapImpl> create() {
    return std::unique_ptr<ResponseHeaderMapImpl>(new (inlineHeadersSize())
                                                      ResponseHeaderMapImpl());
  }