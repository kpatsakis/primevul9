std::string normalize_decimals(const std::string& str) {
  std::string normalized;
  if (!str.empty() && str[0] == '.') {
    normalized.reserve(str.size() + 1);
    normalized += '0';
    normalized += str;
  } else {
    normalized = str;
  }
  return normalized;
}