  absl::optional<StaticLookupResponse> staticLookup(absl::string_view key) override {
    return StaticLookupTable<Interface>::lookup(*this, key);
  }