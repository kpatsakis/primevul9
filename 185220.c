  void addCopy(const LowerCaseString& key, uint64_t value) override {
    header_map_->addCopy(key, value);
    header_map_->verifyByteSizeInternalForTest();
  }