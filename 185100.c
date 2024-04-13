absl::optional<CelValue> convertHeaderEntry(const Http::HeaderEntry* header) {
  if (header == nullptr) {
    return {};
  }
  return CelValue::CreateStringView(header->value().getStringView());
}