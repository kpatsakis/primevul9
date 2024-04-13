Config::Config(const envoy::extensions::filters::http::header_to_metadata::v3::Config config,
               const bool per_route) {
  request_set_ = Config::configToVector(config.request_rules(), request_rules_);
  response_set_ = Config::configToVector(config.response_rules(), response_rules_);

  // Note: empty configs are fine for the global config, which would be the case for enabling
  //       the filter globally without rules and then applying them at the virtual host or
  //       route level. At the virtual or route level, it makes no sense to have an empty
  //       config so we throw an error.
  if (per_route && !response_set_ && !request_set_) {
    throw EnvoyException("header_to_metadata_filter: Per filter configs must at least specify "
                         "either request or response rules");
  }
}