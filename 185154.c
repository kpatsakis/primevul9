Rule::Rule(const ProtoRule& rule) : rule_(rule) {
  // Ensure only one of header and cookie is specified.
  // TODO(radha13): remove this once we are on v4 and these fields are folded into a oneof.
  if (!rule.cookie().empty() && !rule.header().empty()) {
    throw EnvoyException("Cannot specify both header and cookie");
  }

  // Initialize the shared pointer.
  if (!rule.header().empty()) {
    selector_ =
        std::make_shared<HeaderValueSelector>(Http::LowerCaseString(rule.header()), rule.remove());
  } else if (!rule.cookie().empty()) {
    selector_ = std::make_shared<CookieValueSelector>(rule.cookie());
  } else {
    throw EnvoyException("One of Cookie or Header option needs to be specified");
  }

  // Rule must have at least one of the `on_header_*` fields set.
  if (!rule.has_on_header_present() && !rule.has_on_header_missing()) {
    const auto& error = fmt::format("header to metadata filter: rule for {} has neither "
                                    "`on_header_present` nor `on_header_missing` set",
                                    selector_->toString());
    throw EnvoyException(error);
  }

  // Ensure value and regex_value_rewrite are not mixed.
  // TODO(rgs1): remove this once we are on v4 and these fields are folded into a oneof.
  if (!rule.on_header_present().value().empty() &&
      rule.on_header_present().has_regex_value_rewrite()) {
    throw EnvoyException("Cannot specify both value and regex_value_rewrite");
  }

  // Remove field is un-supported for cookie.
  if (!rule.cookie().empty() && rule.remove()) {
    throw EnvoyException("Cannot specify remove for cookie");
  }

  if (rule.has_on_header_missing() && rule.on_header_missing().value().empty()) {
    throw EnvoyException("Cannot specify on_header_missing rule with an empty value");
  }

  if (rule.on_header_present().has_regex_value_rewrite()) {
    const auto& rewrite_spec = rule.on_header_present().regex_value_rewrite();
    regex_rewrite_ = Regex::Utility::parseRegex(rewrite_spec.pattern());
    regex_rewrite_substitution_ = rewrite_spec.substitution();
  }
}