TEST(HeaderMapImplTest, DoubleInlineSet) {
  TestRequestHeaderMapImpl headers;
  headers.setReferenceKey(Headers::get().ContentType, "blah");
  headers.setReferenceKey(Headers::get().ContentType, "text/html");
  EXPECT_EQ("text/html", headers.getContentTypeValue());
  EXPECT_EQ(1UL, headers.size());
}