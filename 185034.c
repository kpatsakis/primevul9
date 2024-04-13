  static std::unique_ptr<RequestTrailerMapImpl> create() {
    return std::unique_ptr<RequestTrailerMapImpl>(new (inlineHeadersSize())
                                                      RequestTrailerMapImpl());
  }