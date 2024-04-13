uint64_t HeaderMapImpl::appendToHeader(HeaderString& header, absl::string_view data,
                                       absl::string_view delimiter) {
  if (data.empty()) {
    return 0;
  }
  uint64_t byte_size = 0;
  if (!header.empty()) {
    header.append(delimiter.data(), delimiter.size());
    byte_size += delimiter.size();
  }
  header.append(data.data(), data.size());
  return data.size() + byte_size;
}