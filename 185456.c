size_t HeaderMapImpl::remove(const LowerCaseString& key) {
  auto lookup = staticLookup(key.get());
  if (lookup.has_value()) {
    const size_t old_size = headers_.size();
    removeInline(lookup.value().entry_);
    return old_size - headers_.size();
  } else {
    // TODO(mattklein123): When the lazy map is implemented we can stop using removeIf() here.
    return HeaderMapImpl::removeIf([&key](const HeaderEntry& entry) -> bool {
      return key.get() == entry.key().getStringView();
    });
  }
}