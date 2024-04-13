void HeaderMapImpl::setReference(const LowerCaseString& key, absl::string_view value) {
  remove(key);
  addReference(key, value);
}