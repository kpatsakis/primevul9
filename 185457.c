void HeaderMapImpl::insertByKey(HeaderString&& key, HeaderString&& value) {
  auto lookup = staticLookup(key.getStringView());
  if (lookup.has_value()) {
    key.clear();
    if (*lookup.value().entry_ == nullptr) {
      maybeCreateInline(lookup.value().entry_, *lookup.value().key_, std::move(value));
    } else {
      const uint64_t added_size =
          appendToHeader((*lookup.value().entry_)->value(), value.getStringView());
      addSize(added_size);
      value.clear();
    }
  } else {
    addSize(key.size() + value.size());
    std::list<HeaderEntryImpl>::iterator i = headers_.insert(std::move(key), std::move(value));
    i->entry_ = i;
  }
}