void HeaderString::setCopy(const char* data, uint32_t size) {
  ASSERT(validHeaderString(absl::string_view(data, size)));

  if (!absl::holds_alternative<InlineHeaderVector>(buffer_)) {
    // Switching from Type::Reference to Type::Inline
    buffer_ = InlineHeaderVector();
  }

  getInVec(buffer_).reserve(size);
  getInVec(buffer_).assign(data, data + size);
  ASSERT(valid());
}