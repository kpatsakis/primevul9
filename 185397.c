  static const MessageType& downcastAndValidate(const Protobuf::Message& config) {
    return MessageUtil::downcastAndValidate<MessageType>(
        config, ProtobufMessage::getStrictValidationVisitor());
  }