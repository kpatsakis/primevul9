  static std::string getProtobufBinaryStringFromMessage(const Protobuf::Message& message) {
    std::string pb_binary_str;
    pb_binary_str.reserve(message.ByteSizeLong());
    message.SerializeToString(&pb_binary_str);
    return pb_binary_str;
  }