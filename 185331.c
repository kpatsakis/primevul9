  void addReferenceKey(const LowerCaseString& key, absl::string_view value) override {
    HeaderMapImpl::addReferenceKey(key, value);
  }