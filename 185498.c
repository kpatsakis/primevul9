void ExtractorImpl::addQueryParamConfig(const std::string& issuer, const std::string& param) {
  auto& param_location_spec = param_locations_[param];
  param_location_spec.specified_issuers_.insert(issuer);
}