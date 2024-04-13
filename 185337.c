  static inline MessageType anyConvert(const ProtobufWkt::Any& message) {
    return MessageUtil::anyConvert<MessageType>(message);
  }