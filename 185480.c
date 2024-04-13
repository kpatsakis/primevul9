  static std::vector<Http::HeaderMatcherSharedPtr> buildHeaderMatcherVector(
      const Protobuf::RepeatedPtrField<envoy::config::route::v3::HeaderMatcher>& header_matchers) {
    std::vector<Http::HeaderMatcherSharedPtr> ret;
    for (const auto& header_matcher : header_matchers) {
      ret.emplace_back(std::make_shared<HeaderUtility::HeaderData>(header_matcher));
    }
    return ret;
  }