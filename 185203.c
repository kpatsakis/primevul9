  void addReference(const LowerCaseString& key, absl::string_view value) override {
    HeaderMapImpl::addReference(key, value);
  }