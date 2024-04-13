createHeaderMap(const std::initializer_list<std::pair<LowerCaseString, std::string>>& values) {
  auto new_header_map = T::create();
  HeaderMapImpl::initFromInitList(*new_header_map, values.begin(), values.end());
  return new_header_map;
}