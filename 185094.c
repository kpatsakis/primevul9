absl::string_view ExtractorImpl::extractJWT(absl::string_view value_str,
                                            absl::string_view::size_type after) const {
  const auto starting = value_str.find_first_of(ConstantBase64UrlEncodingCharsPlusDot, after);
  if (starting == value_str.npos) {
    return value_str;
  }
  // There should be two dots (periods; 0x2e) inside the string, but we don't verify that here
  auto ending = value_str.find_first_not_of(ConstantBase64UrlEncodingCharsPlusDot, starting);
  if (ending == value_str.npos) { // Base64Url-encoded string occupies the rest of the line
    return value_str.substr(starting);
  }
  return value_str.substr(starting, ending - starting);
}