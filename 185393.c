template <> HeaderMapImpl::StaticLookupTable<RequestHeaderMap>::StaticLookupTable() {
#define REGISTER_DEFAULT_REQUEST_HEADER(name)                                                      \
  CustomInlineHeaderRegistry::registerInlineHeader<RequestHeaderMap::header_map_type>(             \
      Headers::get().name);
  INLINE_REQ_HEADERS(REGISTER_DEFAULT_REQUEST_HEADER)
  INLINE_REQ_RESP_HEADERS(REGISTER_DEFAULT_REQUEST_HEADER)

  finalizeTable();

  // Special case where we map a legacy host header to :authority.
  const auto handle =
      CustomInlineHeaderRegistry::getInlineHeader<RequestHeaderMap::header_map_type>(
          Headers::get().Host);
  add(Headers::get().HostLegacy.get().c_str(), [handle](HeaderMapImpl& h) -> StaticLookupResponse {
    return {&h.inlineHeaders()[handle.value().it_->second], &handle.value().it_->first};
  });
}