  void lower() {
    std::transform(string_.begin(), string_.end(), string_.begin(), absl::ascii_tolower);
  }