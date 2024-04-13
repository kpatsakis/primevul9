  void addReferenceKey(const LowerCaseString& key, absl::string_view value) override {
    header_map_->addReferenceKey(key, value);
    header_map_->verifyByteSizeInternalForTest();
  }