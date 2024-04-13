absl::string_view getStrView(const VariantHeader& buffer) {
  return absl::get<absl::string_view>(buffer);
}