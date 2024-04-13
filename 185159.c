TEST(HeaderMapImplTest, TestRequestHeaderMapImplyCopy) {
  TestRequestHeaderMapImpl foo;
  foo.addCopy(LowerCaseString("foo"), "bar");
  auto headers = std::make_unique<TestRequestHeaderMapImpl>(foo);
  EXPECT_EQ("bar", headers->get(LowerCaseString("foo"))->value().getStringView());
  TestRequestHeaderMapImpl baz{{"foo", "baz"}};
  baz = *headers;
  EXPECT_EQ("bar", baz.get(LowerCaseString("foo"))->value().getStringView());
  const TestRequestHeaderMapImpl& baz2 = baz;
  baz = baz2;
  EXPECT_EQ("bar", baz.get(LowerCaseString("foo"))->value().getStringView());
}