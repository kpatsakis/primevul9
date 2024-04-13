void ExtractorImpl::addHeaderConfig(const std::string& issuer, const LowerCaseString& header_name,
                                    const std::string& value_prefix) {
  ENVOY_LOG(debug, "addHeaderConfig for issuer {} at {}", issuer, header_name.get());
  const std::string map_key = header_name.get() + value_prefix;
  auto& header_location_spec = header_locations_[map_key];
  if (!header_location_spec) {
    header_location_spec = std::make_unique<HeaderLocationSpec>(header_name, value_prefix);
  }
  header_location_spec->specified_issuers_.insert(issuer);
}