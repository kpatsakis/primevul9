  static const envoy::type::matcher::v3::StringMatcher createRegexMatcher(std::string str) {
    envoy::type::matcher::v3::StringMatcher matcher;
    matcher.set_hidden_envoy_deprecated_regex(str);
    return matcher;
  }