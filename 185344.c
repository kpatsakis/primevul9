  static void loadFromJson(const std::string& json, Protobuf::Message& message,
                           bool preserve_original_type = false, bool avoid_boosting = false) {
    MessageUtil::loadFromJson(json, message, ProtobufMessage::getStrictValidationVisitor(),
                              !avoid_boosting);
    if (!preserve_original_type) {
      Config::VersionConverter::eraseOriginalTypeInformation(message);
    }
  }