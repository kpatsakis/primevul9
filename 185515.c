  envoy::config::core::v3::Metadata parseMetadataFromYaml(const std::string& yaml_string) {
    envoy::config::core::v3::Metadata metadata;
    TestUtility::loadFromYaml(yaml_string, metadata);
    return metadata;
  }