convertHeaderEntry(Protobuf::Arena& arena,
                   Http::HeaderUtility::GetAllOfHeaderAsStringResult&& result) {
  if (!result.result().has_value()) {
    return {};
  } else if (!result.backingString().empty()) {
    return CelValue::CreateString(
        Protobuf::Arena::Create<std::string>(&arena, result.backingString()));
  } else {
    return CelValue::CreateStringView(result.result().value());
  }
}