  decodeResources(const envoy::service::discovery::v3::DiscoveryResponse& resources,
                  const std::string& name_field = "name") {
    return decodeResources<MessageType>(resources.resources(), resources.version_info(),
                                        name_field);
  }