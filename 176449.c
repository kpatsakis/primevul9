std::string normalize_newlines(const std::string& str) {
  std::string result;
  result.reserve(str.size());
  std::size_t pos = 0;
  while (true) {
    const std::size_t newline = str.find_first_of("\n\f\r", pos);
    if (newline == std::string::npos) break;
    result.append(str, pos, newline - pos);
    result += '\n';
    if (str[newline] == '\r' && str[newline + 1] == '\n') {
      pos = newline + 2;
    } else {
      pos = newline + 1;
    }
  }
  result.append(str, pos, std::string::npos);
  return result;
}