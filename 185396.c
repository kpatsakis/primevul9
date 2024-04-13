absl::optional<CelValue> FilterStateWrapper::operator[](CelValue key) const {
  if (!key.IsString()) {
    return {};
  }
  auto value = key.StringOrDie().value();
  if (filter_state_.hasDataWithName(value)) {
    const StreamInfo::FilterState::Object* object = filter_state_.getDataReadOnlyGeneric(value);
    absl::optional<std::string> serialized = object->serializeAsString();
    if (serialized.has_value()) {
      std::string* out = ProtobufWkt::Arena::Create<std::string>(arena_, serialized.value());
      return CelValue::CreateBytes(out);
    }
  }
  return {};
}