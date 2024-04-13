template <class T, class It> std::unique_ptr<T> createHeaderMap(It begin, It end) {
  auto new_header_map = T::create();
  HeaderMapImpl::initFromInitList(*new_header_map, begin, end);
  return new_header_map;
}