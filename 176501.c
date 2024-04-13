std::string rtrim(const std::string &str) {
  std::string trimmed = str;
  size_t pos_ws = trimmed.find_last_not_of(" \t\n\v\f\r");
  if (pos_ws != std::string::npos) {
    trimmed.erase(pos_ws + 1);
  } else {
    trimmed.clear();
  }
  return trimmed;
}