  static void loadFromYaml(const std::string& yaml, Protobuf::Message& message,
                           bool preserve_original_type = false, bool avoid_boosting = false) {
    MessageUtil::loadFromYaml(yaml, message, ProtobufMessage::getStrictValidationVisitor(),
                              !avoid_boosting);
    if (!preserve_original_type) {
      Config::VersionConverter::eraseOriginalTypeInformation(message);
    }
  }