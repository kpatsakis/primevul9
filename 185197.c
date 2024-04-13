HeaderUtility::requestHeadersValid(const RequestHeaderMap& headers) {
  // Make sure the host is valid.
  if (headers.Host() && !HeaderUtility::authorityIsValid(headers.Host()->value().getStringView())) {
    return SharedResponseCodeDetails::get().InvalidAuthority;
  }
  return absl::nullopt;
}