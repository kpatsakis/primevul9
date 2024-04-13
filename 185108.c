  size_t removePrefix(const LowerCaseString& key) override {
    size_t headers_removed = header_map_->removePrefix(key);
    header_map_->verifyByteSizeInternalForTest();
    return headers_removed;
  }