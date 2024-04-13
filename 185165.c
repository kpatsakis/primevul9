  static void jsonConvert(const Protobuf::Message& source, Protobuf::Message& dest) {
    // Explicit round-tripping to support conversions inside tests between arbitrary messages as a
    // convenience.
    ProtobufWkt::Struct tmp;
    MessageUtil::jsonConvert(source, tmp);
    MessageUtil::jsonConvert(tmp, ProtobufMessage::getStrictValidationVisitor(), dest);
  }