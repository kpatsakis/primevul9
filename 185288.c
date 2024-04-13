makeHeaderMap(const std::initializer_list<std::pair<std::string, std::string>>& values) {
  return std::make_unique<T, const std::initializer_list<std::pair<std::string, std::string>>&>(
      values);
}