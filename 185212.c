    static absl::optional<StaticLookupResponse> lookup(HeaderMapImpl& header_map,
                                                       absl::string_view key) {
      const auto& entry = ConstSingleton<StaticLookupTable>::get().find(key);
      if (entry != nullptr) {
        return entry(header_map);
      } else {
        return absl::nullopt;
      }
    }