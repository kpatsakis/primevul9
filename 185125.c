  void addCopy(const LowerCaseString& key, absl::string_view value) override {
    HeaderMapImpl::addCopy(key, value);
  }