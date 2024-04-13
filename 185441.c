  size_t removeIf(const HeaderMap::HeaderMatchPredicate& predicate) override {
    size_t headers_removed = header_map_->removeIf(predicate);
    header_map_->verifyByteSizeInternalForTest();
    return headers_removed;
  }