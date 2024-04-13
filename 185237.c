TEST(HeaderMapImplTest, IterateReverse) {
  TestRequestHeaderMapImpl headers;
  headers.addCopy(LowerCaseString("hello"), "world");
  headers.addCopy(LowerCaseString("foo"), "bar");
  LowerCaseString world_key("world");
  headers.setReferenceKey(world_key, "hello");

  HeaderAndValueCb cb;

  InSequence seq;
  EXPECT_CALL(cb, Call("world", "hello"));
  EXPECT_CALL(cb, Call("foo", "bar"));
  // no "hello"
  headers.iterateReverse([&cb](const Http::HeaderEntry& header) -> HeaderMap::Iterate {
    cb.Call(std::string(header.key().getStringView()), std::string(header.value().getStringView()));
    if (header.key().getStringView() != "foo") {
      return HeaderMap::Iterate::Continue;
    } else {
      return HeaderMap::Iterate::Break;
    }
  });
}