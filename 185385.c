  void addReference(const LowerCaseString& key, absl::string_view value) override {
    header_map_->addReference(key, value);
    header_map_->verifyByteSizeInternalForTest();
  }