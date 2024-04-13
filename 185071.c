TEST(HeaderMapImplTest, RemovePrefix) {
  // These will match.
  LowerCaseString key1 = LowerCaseString("X-prefix-foo");
  LowerCaseString key3 = LowerCaseString("X-Prefix-");
  LowerCaseString key5 = LowerCaseString("x-prefix-eep");
  // These will not.
  LowerCaseString key2 = LowerCaseString(" x-prefix-foo");
  LowerCaseString key4 = LowerCaseString("y-x-prefix-foo");

  TestRequestHeaderMapImpl headers;
  headers.addReference(key1, "value");
  headers.addReference(key2, "value");
  headers.addReference(key3, "value");
  headers.addReference(key4, "value");
  headers.addReference(key5, "value");

  // Test removing the first header, middle headers, and the end header.
  EXPECT_EQ(3UL, headers.removePrefix(LowerCaseString("x-prefix-")));
  EXPECT_EQ(nullptr, headers.get(key1));
  EXPECT_NE(nullptr, headers.get(key2));
  EXPECT_EQ(nullptr, headers.get(key3));
  EXPECT_NE(nullptr, headers.get(key4));
  EXPECT_EQ(nullptr, headers.get(key5));

  // Try to remove headers with no prefix match.
  EXPECT_EQ(0UL, headers.removePrefix(LowerCaseString("foo")));

  // Remove all headers.
  EXPECT_EQ(2UL, headers.removePrefix(LowerCaseString("")));
  EXPECT_EQ(nullptr, headers.get(key2));
  EXPECT_EQ(nullptr, headers.get(key4));

  // Add inline and remove by prefix
  headers.setContentLength(5);
  EXPECT_EQ("5", headers.getContentLengthValue());
  EXPECT_EQ(1UL, headers.size());
  EXPECT_FALSE(headers.empty());
  EXPECT_EQ(1UL, headers.removePrefix(LowerCaseString("content")));
  EXPECT_EQ(nullptr, headers.ContentLength());
}