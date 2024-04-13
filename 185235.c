  static bool jsonStringEqual(const std::string& lhs, const std::string& rhs) {
    return protoEqual(jsonToStruct(lhs), jsonToStruct(rhs));
  }