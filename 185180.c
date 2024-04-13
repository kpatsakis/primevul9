  void setCopy(const LowerCaseString& key, absl::string_view value) override {
    header_map_->setCopy(key, value);
    header_map_->verifyByteSizeInternalForTest();
  }