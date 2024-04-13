  static bool protoEqual(const Protobuf::Message& lhs, const Protobuf::Message& rhs,
                         bool ignore_repeated_field_ordering = false) {
    Protobuf::util::MessageDifferencer differencer;
    differencer.set_message_field_comparison(Protobuf::util::MessageDifferencer::EQUIVALENT);
    if (ignore_repeated_field_ordering) {
      differencer.set_repeated_field_comparison(Protobuf::util::MessageDifferencer::AS_SET);
    }
    return differencer.Compare(lhs, rhs);
  }