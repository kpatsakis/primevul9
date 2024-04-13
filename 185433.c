HeaderMap::NonConstGetResult HeaderMapImpl::getExisting(const LowerCaseString& key) {
  // Attempt a trie lookup first to see if the user is requesting an O(1) header. This may be
  // relatively common in certain header matching / routing patterns.
  // TODO(mattklein123): Add inline handle support directly to the header matcher code to support
  // this use case more directly.
  HeaderMap::NonConstGetResult ret;
  auto lookup = staticLookup(key.get());
  if (lookup.has_value()) {
    if (*lookup.value().entry_ != nullptr) {
      ret.push_back(*lookup.value().entry_);
    }
    return ret;
  }

  // If the requested header is not an O(1) header we do a full scan. Doing the trie lookup is
  // wasteful in the miss case, but is present for code consistency with other functions that do
  // similar things.
  // TODO(mattklein123): The full scan here and in remove() are the biggest issues with this
  // implementation for certain use cases. We can either replace this with a totally different
  // implementation or potentially create a lazy map if the size of the map is above a threshold.
  for (HeaderEntryImpl& header : headers_) {
    if (header.key() == key.get().c_str()) {
      ret.push_back(&header);
    }
  }

  return ret;
}