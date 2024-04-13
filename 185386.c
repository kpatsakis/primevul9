  static const envoy::type::matcher::v3::StringMatcher createPrefixMatcher(std::string str) {
    envoy::type::matcher::v3::StringMatcher matcher;
    matcher.set_prefix(str);
    return matcher;
  }