void HeaderString::setReference(absl::string_view ref_value) {
  buffer_ = ref_value;
  ASSERT(valid());
}