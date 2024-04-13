const HeaderEntry* HeaderMapImpl::get(const LowerCaseString& key) const {
  const auto result = getAll(key);
  return result.empty() ? nullptr : result[0];
}