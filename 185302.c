TEST(HeaderMapImplTest, AddReferenceKey) {
  TestRequestHeaderMapImpl headers;
  LowerCaseString foo("hello");
  headers.addReferenceKey(foo, "world");
  EXPECT_NE("world", headers.get(foo)->value().getStringView().data());
  EXPECT_EQ("world", headers.get(foo)->value().getStringView());
}