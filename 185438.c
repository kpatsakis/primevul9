  void setReferenceKey(const LowerCaseString& key, absl::string_view value) override {
    header_map_->setReferenceKey(key, value);
  }