  void setReference(const LowerCaseString& key, absl::string_view value) override {
    header_map_->setReference(key, value);
    header_map_->verifyByteSizeInternalForTest();
  }