TEST(HeaderMapImplTest, CustomRegisteredHeaders) {
  TestRequestHeaderMapImpl headers;
  EXPECT_EQ(custom_header_1.handle(), custom_header_1_copy.handle());
  EXPECT_EQ(nullptr, headers.getInline(custom_header_1.handle()));
  EXPECT_EQ(nullptr, headers.getInline(custom_header_1_copy.handle()));
  headers.setInline(custom_header_1.handle(), 42);
  EXPECT_EQ("42", headers.getInlineValue(custom_header_1_copy.handle()));
  EXPECT_EQ("foo_custom_header",
            headers.getInline(custom_header_1.handle())->key().getStringView());
}