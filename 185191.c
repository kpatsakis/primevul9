  decodeResources(const Protobuf::RepeatedPtrField<ProtobufWkt::Any>& resources,
                  const std::string& version, const std::string& name_field = "name") {
    TestOpaqueResourceDecoderImpl<MessageType> resource_decoder(name_field);
    return Config::DecodedResourcesWrapper(resource_decoder, resources, version);
  }