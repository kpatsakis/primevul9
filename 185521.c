TEST(HeaderMapImplTest, DoubleInlineAdd) {
  {
    TestRequestHeaderMapImpl headers;
    const std::string foo("foo");
    const std::string bar("bar");
    headers.addReference(Headers::get().ContentLength, foo);
    headers.addReference(Headers::get().ContentLength, bar);
    EXPECT_EQ("foo,bar", headers.getContentLengthValue());
    EXPECT_EQ(1UL, headers.size());
  }
  {
    TestRequestHeaderMapImpl headers;
    headers.addReferenceKey(Headers::get().ContentLength, "foo");
    headers.addReferenceKey(Headers::get().ContentLength, "bar");
    EXPECT_EQ("foo,bar", headers.getContentLengthValue());
    EXPECT_EQ(1UL, headers.size());
  }
  {
    TestRequestHeaderMapImpl headers;
    headers.addReferenceKey(Headers::get().ContentLength, 5);
    headers.addReferenceKey(Headers::get().ContentLength, 6);
    EXPECT_EQ("5,6", headers.getContentLengthValue());
    EXPECT_EQ(1UL, headers.size());
  }
  {
    TestRequestHeaderMapImpl headers;
    const std::string foo("foo");
    headers.addReference(Headers::get().ContentLength, foo);
    headers.addReferenceKey(Headers::get().ContentLength, 6);
    EXPECT_EQ("foo,6", headers.getContentLengthValue());
    EXPECT_EQ(1UL, headers.size());
  }
}