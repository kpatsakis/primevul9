  static Config::DecodedResourcesWrapper decodeResources(
      const Protobuf::RepeatedPtrField<envoy::service::discovery::v3::Resource>& resources,
      const std::string& name_field = "name") {
    Config::DecodedResourcesWrapper decoded_resources;
    TestOpaqueResourceDecoderImpl<MessageType> resource_decoder(name_field);
    for (const auto& resource : resources) {
      decoded_resources.owned_resources_.emplace_back(
          new Config::DecodedResourceImpl(resource_decoder, resource));
      decoded_resources.refvec_.emplace_back(*decoded_resources.owned_resources_.back());
    }
    return decoded_resources;
  }