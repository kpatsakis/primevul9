  template <class MessageType> static void validate(const MessageType& message) {
    MessageUtil::validate(message, ProtobufMessage::getStrictValidationVisitor());
  }