void HeaderMapImpl::appendCopy(const LowerCaseString& key, absl::string_view value) {
  // TODO(#9221): converge on and document a policy for coalescing multiple headers.
  auto entry = getExisting(key);
  if (!entry.empty()) {
    const uint64_t added_size = appendToHeader(entry[0]->value(), value);
    addSize(added_size);
  } else {
    addCopy(key, value);
  }
}