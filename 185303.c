absl::optional<std::string> CookieValueSelector::extract(Http::HeaderMap& map) const {
  std::string value = Envoy::Http::Utility::parseCookieValue(map, cookie_);
  if (!value.empty()) {
    return absl::optional<std::string>(std::move(value));
  }
  return absl::nullopt;
}