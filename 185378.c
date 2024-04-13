  void setCopy(const LowerCaseString& key, absl::string_view value) override {
    HeaderMapImpl::setCopy(key, value);
  }