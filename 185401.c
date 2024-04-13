  void addViaMove(HeaderString&& key, HeaderString&& value) override {
    HeaderMapImpl::addViaMove(std::move(key), std::move(value));
  }