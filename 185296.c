  bool operator!=(const char* rhs) const {
    return getStringView() != absl::NullSafeStringView(rhs);
  }