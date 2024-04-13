envoy::config::route::v3::HeaderMatcher parseHeaderMatcherFromYaml(const std::string& yaml) {
  envoy::config::route::v3::HeaderMatcher header_matcher;
  TestUtility::loadFromYaml(yaml, header_matcher);
  return header_matcher;
}