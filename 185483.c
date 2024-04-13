static inline bool validHeaderString(absl::string_view s) {
  // If you modify this list of illegal embedded characters you will probably
  // want to change header_map_fuzz_impl_test at the same time.
  for (const char c : s) {
    switch (c) {
    case '\0':
      FALLTHRU;
    case '\r':
      FALLTHRU;
    case '\n':
      return false;
    default:
      continue;
    }
  }
  return true;
}