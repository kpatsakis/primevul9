  static ProtobufWkt::Struct jsonToStruct(const std::string& json) {
    ProtobufWkt::Struct message;
    MessageUtil::loadFromJson(json, message);
    return message;
  }