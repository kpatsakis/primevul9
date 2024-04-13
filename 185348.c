bool HeaderMapImpl::operator==(const HeaderMap& rhs) const {
  if (size() != rhs.size()) {
    return false;
  }

  std::vector<std::pair<absl::string_view, absl::string_view>> rhs_headers;
  rhs_headers.reserve(rhs.size());
  rhs.iterate(collectAllHeaders(&rhs_headers));

  auto i = headers_.begin();
  auto j = rhs_headers.begin();
  for (; i != headers_.end(); ++i, ++j) {
    if (i->key() != j->first || i->value() != j->second) {
      return false;
    }
  }

  return true;
}