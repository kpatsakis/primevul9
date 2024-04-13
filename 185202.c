  absl::string_view getInlineValue(Handle handle) const {
    const auto header = getInline(handle);
    if (header != nullptr) {
      return header->value().getStringView();
    }
    return {};
  }