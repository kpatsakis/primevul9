TEST(HeaderMapImplTest, AllInlineHeaders) {
  {
    auto header_map = RequestHeaderMapImpl::create();
    INLINE_REQ_HEADERS(TEST_INLINE_HEADER_FUNCS)
    INLINE_REQ_RESP_HEADERS(TEST_INLINE_HEADER_FUNCS)
  }
  {
      // No request trailer O(1) headers.
  } {
    auto header_map = ResponseHeaderMapImpl::create();
    INLINE_RESP_HEADERS(TEST_INLINE_HEADER_FUNCS)
    INLINE_REQ_RESP_HEADERS(TEST_INLINE_HEADER_FUNCS)
    INLINE_RESP_HEADERS_TRAILERS(TEST_INLINE_HEADER_FUNCS)
  }
  {
    auto header_map = ResponseTrailerMapImpl::create();
    INLINE_RESP_HEADERS_TRAILERS(TEST_INLINE_HEADER_FUNCS)
  }
}