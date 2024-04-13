  void appendCopy(const LowerCaseString& key, absl::string_view value) override {
    header_map_->appendCopy(key, value);
    header_map_->verifyByteSizeInternalForTest();
  }