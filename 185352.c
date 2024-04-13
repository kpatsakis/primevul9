size_t HeaderMapImpl::removePrefix(const LowerCaseString& prefix) {
  return HeaderMapImpl::removeIf([&prefix](const HeaderEntry& entry) -> bool {
    return absl::StartsWith(entry.key().getStringView(), prefix.get());
  });
}