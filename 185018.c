void ExtractorImpl::addProvider(const JwtProvider& provider) {
  for (const auto& header : provider.from_headers()) {
    addHeaderConfig(provider.issuer(), LowerCaseString(header.name()), header.value_prefix());
  }
  for (const std::string& param : provider.from_params()) {
    addQueryParamConfig(provider.issuer(), param);
  }
  // If not specified, use default locations.
  if (provider.from_headers().empty() && provider.from_params().empty()) {
    addHeaderConfig(provider.issuer(), Http::CustomHeaders::get().Authorization,
                    JwtConstValues::get().BearerPrefix);
    addQueryParamConfig(provider.issuer(), JwtConstValues::get().AccessTokenParam);
  }
  if (!provider.forward_payload_header().empty()) {
    forward_payload_headers_.emplace_back(provider.forward_payload_header());
  }
}