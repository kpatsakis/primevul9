void HeaderMapImpl::addCopy(const LowerCaseString& key, uint64_t value) {
  // In the case that the header is appended, we will perform a needless copy of the key and value.
  // This is done on purpose to keep the code simple and should be rare.
  HeaderString new_key;
  new_key.setCopy(key.get());
  HeaderString new_value;
  new_value.setInteger(value);
  insertByKey(std::move(new_key), std::move(new_value));
  ASSERT(new_key.empty());   // NOLINT(bugprone-use-after-move)
  ASSERT(new_value.empty()); // NOLINT(bugprone-use-after-move)
}