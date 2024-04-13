HeaderMapImplUtility::HeaderMapImplInfo makeHeaderMapImplInfo(absl::string_view name) {
  // Constructing a header map implementation will force the custom headers and sizing to be
  // finalized, so do that first.
  auto header_map = T::create();

  HeaderMapImplUtility::HeaderMapImplInfo info;
  info.name_ = std::string(name);
  info.size_ = T::inlineHeadersSize() + sizeof(T);
  for (const auto& header : CustomInlineHeaderRegistry::headers<T::header_map_type>()) {
    info.registered_headers_.push_back(header.first.get());
  }
  return info;
}