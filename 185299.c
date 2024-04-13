template <class T> std::unique_ptr<T> createHeaderMap(const HeaderMap& rhs) {
  // TODO(mattklein123): Use of this function allows copying a request header map into a response
  // header map, etc. which is probably not what we want. Unfortunately, we do this on purpose in
  // a few places when dealing with gRPC headers/trailers conversions so it's not trivial to remove.
  // We should revisit this to figure how to make this a bit safer as a non-intentional conversion
  // may have surprising results with different O(1) headers, implementations, etc.
  auto new_header_map = T::create();
  HeaderMapImpl::copyFrom(*new_header_map, rhs);
  return new_header_map;
}