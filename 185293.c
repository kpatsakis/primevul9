TEST(HeaderMapImplTest, Iterate) {
  TestRequestHeaderMapImpl headers;
  headers.addCopy(LowerCaseString("hello"), "world");
  headers.addCopy(LowerCaseString("foo"), "xxx");
  headers.addCopy(LowerCaseString("world"), "hello");
  LowerCaseString foo_key("foo");
  headers.setReferenceKey(foo_key, "bar"); // set moves key to end

  HeaderAndValueCb cb;

  InSequence seq;
  EXPECT_CALL(cb, Call("hello", "world"));
  EXPECT_CALL(cb, Call("world", "hello"));
  EXPECT_CALL(cb, Call("foo", "bar"));
  headers.iterate(cb.asIterateCb());
}