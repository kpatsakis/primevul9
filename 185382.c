void HeaderMapImpl::addReferenceKey(const LowerCaseString& key, absl::string_view value) {
  HeaderString ref_key(key);
  HeaderString new_value;
  new_value.setCopy(value);
  insertByKey(std::move(ref_key), std::move(new_value));
  ASSERT(new_value.empty()); // NOLINT(bugprone-use-after-move)
}