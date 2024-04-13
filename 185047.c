  void addCopy(const LowerCaseString& key, absl::string_view value) override {
    header_map_->addCopy(key, value);
    header_map_->verifyByteSizeInternalForTest();
  }