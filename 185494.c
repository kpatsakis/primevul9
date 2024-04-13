  static void loadFromYamlAndValidate(const std::string& yaml, MessageType& message,
                                      bool preserve_original_type = false,
                                      bool avoid_boosting = false) {
    MessageUtil::loadFromYamlAndValidate(
        yaml, message, ProtobufMessage::getStrictValidationVisitor(), avoid_boosting);
    if (!preserve_original_type) {
      Config::VersionConverter::eraseOriginalTypeInformation(message);
    }
  }