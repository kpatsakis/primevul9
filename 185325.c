template <> HeaderMapImpl::StaticLookupTable<ResponseHeaderMap>::StaticLookupTable() {
#define REGISTER_RESPONSE_HEADER(name)                                                             \
  CustomInlineHeaderRegistry::registerInlineHeader<ResponseHeaderMap::header_map_type>(            \
      Headers::get().name);
  INLINE_RESP_HEADERS(REGISTER_RESPONSE_HEADER)
  INLINE_REQ_RESP_HEADERS(REGISTER_RESPONSE_HEADER)
  INLINE_RESP_HEADERS_TRAILERS(REGISTER_RESPONSE_HEADER)

  finalizeTable();
}