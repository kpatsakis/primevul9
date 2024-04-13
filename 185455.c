  static void loadFromFile(const std::string& path, Protobuf::Message& message, Api::Api& api,
                           bool preserve_original_type = false) {
    MessageUtil::loadFromFile(path, message, ProtobufMessage::getStrictValidationVisitor(), api);
    if (!preserve_original_type) {
      Config::VersionConverter::eraseOriginalTypeInformation(message);
    }
  }