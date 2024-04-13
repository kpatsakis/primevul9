std::string normalize_underscores(const std::string& str) {
  std::string normalized = str;
  std::replace(normalized.begin(), normalized.end(), '_', '-');
  return normalized;
}