  static Config::DecodedResourcesWrapper decodeResources(std::vector<MessageType> resources,
                                                         const std::string& name_field = "name") {
    Config::DecodedResourcesWrapper decoded_resources;
    for (const auto& resource : resources) {
      auto owned_resource = std::make_unique<MessageType>(resource);
      decoded_resources.owned_resources_.emplace_back(new Config::DecodedResourceImpl(
          std::move(owned_resource), MessageUtil::getStringField(resource, name_field), {}, ""));
      decoded_resources.refvec_.emplace_back(*decoded_resources.owned_resources_.back());
    }
    return decoded_resources;
  }