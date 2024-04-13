  getVersionedServiceFullName(const std::string& service_full_name_template,
                              envoy::config::core::v3::ApiVersion api_version,
                              bool use_alpha = false,
                              const std::string& service_namespace = EMPTY_STRING) {
    switch (api_version) {
    case envoy::config::core::v3::ApiVersion::AUTO:
      FALLTHRU;
    case envoy::config::core::v3::ApiVersion::V2:
      return fmt::format(service_full_name_template, use_alpha ? "v2alpha" : "v2",
                         service_namespace);

    case envoy::config::core::v3::ApiVersion::V3:
      return fmt::format(service_full_name_template, "v3", service_namespace);
    default:
      NOT_REACHED_GCOVR_EXCL_LINE;
    }
  }