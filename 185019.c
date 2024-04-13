TEST(HeaderMapImplTest, ClearHeaderMap) {
  TestRequestHeaderMapImpl headers;
  LowerCaseString static_key("hello");
  std::string ref_value("value");

  // Add random header and then clear.
  headers.addReference(static_key, ref_value);
  EXPECT_EQ("value", headers.get(static_key)->value().getStringView());
  EXPECT_TRUE(headers.get(static_key)->value().isReference());
  EXPECT_EQ(1UL, headers.size());
  EXPECT_FALSE(headers.empty());
  headers.clear();
  EXPECT_EQ(nullptr, headers.get(static_key));
  EXPECT_EQ(0UL, headers.size());
  EXPECT_EQ(headers.byteSize(), 0);
  EXPECT_TRUE(headers.empty());

  // Add inline and clear.
  headers.setContentLength(5);
  EXPECT_EQ("5", headers.getContentLengthValue());
  EXPECT_EQ(1UL, headers.size());
  EXPECT_FALSE(headers.empty());
  headers.clear();
  EXPECT_EQ(nullptr, headers.ContentLength());
  EXPECT_EQ(0UL, headers.size());
  EXPECT_EQ(headers.byteSize(), 0);
  EXPECT_TRUE(headers.empty());

  // Add mixture of headers.
  headers.addReference(static_key, ref_value);
  headers.setContentLength(5);
  headers.addCopy(static_key, "new_value");
  EXPECT_EQ(3UL, headers.size());
  EXPECT_FALSE(headers.empty());
  headers.clear();
  EXPECT_EQ(nullptr, headers.ContentLength());
  EXPECT_EQ(0UL, headers.size());
  EXPECT_EQ(headers.byteSize(), 0);
  EXPECT_TRUE(headers.empty());
}