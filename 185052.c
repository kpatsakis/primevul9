absl::optional<std::string> HeaderValueSelector::extract(Http::HeaderMap& map) const {
  const auto header_value = Http::HeaderUtility::getAllOfHeaderAsString(map, header_);
  if (!header_value.result().has_value()) {
    return absl::nullopt;
  }
  // Catch the value in the header before removing.
  absl::optional<std::string> value = std::string(header_value.result().value());
  if (remove_) {
    map.remove(header_);
  }
  return value;
}