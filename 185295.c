  static const envoy::type::matcher::v3::StringMatcher createExactMatcher(std::string str) {
    envoy::type::matcher::v3::StringMatcher matcher;
    matcher.set_exact(str);
    return matcher;
  }