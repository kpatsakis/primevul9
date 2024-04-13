  template <class MessageType> static MessageType parseYaml(const std::string& yaml) {
    MessageType message;
    TestUtility::loadFromYaml(yaml, message);
    return message;
  }