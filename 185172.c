  void appendCopy(const LowerCaseString& key, absl::string_view value) override {
    HeaderMapImpl::appendCopy(key, value);
  }