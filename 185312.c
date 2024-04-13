void HeaderMapImpl::setReferenceKey(const LowerCaseString& key, absl::string_view value) {
  remove(key);
  addReferenceKey(key, value);
}