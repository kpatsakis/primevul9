  static absl::optional<Handle<type>> getInlineHeader(const LowerCaseString& header_name) {
    ASSERT(mutableFinalized<type>());
    auto& map = mutableRegistrationMap<type>();
    auto entry = map.find(header_name);
    if (entry != map.end()) {
      return Handle<type>(entry);
    }
    return absl::nullopt;
  }