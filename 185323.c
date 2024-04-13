TEST(HeaderMapImplTest, Get) {
  {
    auto headers = TestRequestHeaderMapImpl({{Headers::get().Path.get(), "/"}, {"hello", "world"}});
    EXPECT_EQ("/", headers.get(LowerCaseString(":path"))->value().getStringView());
    EXPECT_EQ("world", headers.get(LowerCaseString("hello"))->value().getStringView());
    EXPECT_EQ(nullptr, headers.get(LowerCaseString("foo")));
  }

  {
    auto headers = TestRequestHeaderMapImpl({{Headers::get().Path.get(), "/"}, {"hello", "world"}});
    // There is not HeaderMap method to set a header and copy both the key and value.
    const LowerCaseString path(":path");
    headers.setReferenceKey(path, "/new_path");
    EXPECT_EQ("/new_path", headers.get(LowerCaseString(":path"))->value().getStringView());
    const LowerCaseString foo("hello");
    headers.setReferenceKey(foo, "world2");
    EXPECT_EQ("world2", headers.get(foo)->value().getStringView());
    EXPECT_EQ(nullptr, headers.get(LowerCaseString("foo")));
  }
}