collectAllHeaders(std::vector<std::pair<absl::string_view, absl::string_view>>* dest) {
  return [dest](const HeaderEntry& header) -> HeaderMap::Iterate {
    dest->push_back(std::make_pair(header.key().getStringView(), header.value().getStringView()));
    return HeaderMap::Iterate::Continue;
  };
};