  static bool protoEqualIgnoringField(const Protobuf::Message& lhs, const Protobuf::Message& rhs,
                                      const std::string& field_to_ignore) {
    Protobuf::util::MessageDifferencer differencer;
    const Protobuf::FieldDescriptor* ignored_field =
        lhs.GetDescriptor()->FindFieldByName(field_to_ignore);
    ASSERT(ignored_field != nullptr, "Field name to ignore not found.");
    differencer.IgnoreField(ignored_field);
    return differencer.Compare(lhs, rhs);
  }