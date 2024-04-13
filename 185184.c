  static bool decodedResourceEq(const Config::DecodedResource& lhs,
                                const Config::DecodedResource& rhs) {
    return lhs.name() == rhs.name() && lhs.aliases() == rhs.aliases() &&
           lhs.version() == rhs.version() && lhs.hasResource() == rhs.hasResource() &&
           (!lhs.hasResource() || protoEqual(lhs.resource(), rhs.resource()));
  }