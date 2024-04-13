  size_t remove(const LowerCaseString& key) override {
    size_t headers_removed = header_map_->remove(key);
    header_map_->verifyByteSizeInternalForTest();
    return headers_removed;
  }