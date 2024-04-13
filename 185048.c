  getVersionStringFromApiVersion(envoy::config::core::v3::ApiVersion api_version) {
    switch (api_version) {
    case envoy::config::core::v3::ApiVersion::AUTO:
      return "AUTO";
    case envoy::config::core::v3::ApiVersion::V2:
      return "V2";
    case envoy::config::core::v3::ApiVersion::V3:
      return "V3";
    default:
      NOT_REACHED_GCOVR_EXCL_LINE;
    }
  }