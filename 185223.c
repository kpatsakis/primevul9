  absl::optional<CelValue> operator[](CelValue key) const override {
    if (value_ == nullptr || !key.IsString()) {
      return {};
    }
    auto str = std::string(key.StringOrDie().value());
    if (!Http::validHeaderString(str)) {
      // Reject key if it is an invalid header string
      return {};
    }
    return convertHeaderEntry(
        arena_, Http::HeaderUtility::getAllOfHeaderAsString(*value_, Http::LowerCaseString(str)));
  }