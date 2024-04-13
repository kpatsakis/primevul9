  void addReferenceKey(const LowerCaseString& key, uint64_t value) override {
    header_map_->addReferenceKey(key, value);
    header_map_->verifyByteSizeInternalForTest();
  }