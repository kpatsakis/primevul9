  static std::unique_ptr<ResponseTrailerMapImpl> create() {
    return std::unique_ptr<ResponseTrailerMapImpl>(new (inlineHeadersSize())
                                                       ResponseTrailerMapImpl());
  }