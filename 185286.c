void HeaderMapImpl::addReference(const LowerCaseString& key, absl::string_view value) {
  HeaderString ref_key(key);
  HeaderString ref_value(value);
  insertByKey(std::move(ref_key), std::move(ref_value));
}