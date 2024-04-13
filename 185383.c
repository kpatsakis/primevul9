  void addViaMove(HeaderString&& key, HeaderString&& value) override {
    header_map_->addViaMove(std::move(key), std::move(value));
    header_map_->verifyByteSizeInternalForTest();
  }