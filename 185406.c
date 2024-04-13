TEST(HeaderMapImplTest, Remove) {
  TestRequestHeaderMapImpl headers;

  // Add random header and then remove by name.
  LowerCaseString static_key("hello");
  std::string ref_value("value");
  headers.addReference(static_key, ref_value);
  EXPECT_EQ("value", headers.get(static_key)->value().getStringView());
  EXPECT_TRUE(headers.get(static_key)->value().isReference());
  EXPECT_EQ(1UL, headers.size());
  EXPECT_FALSE(headers.empty());
  EXPECT_EQ(1UL, headers.remove(static_key));
  EXPECT_EQ(nullptr, headers.get(static_key));
  EXPECT_EQ(0UL, headers.size());
  EXPECT_TRUE(headers.empty());

  // Add and remove by inline.
  EXPECT_EQ(0UL, headers.removeContentLength());
  headers.setContentLength(5);
  EXPECT_EQ("5", headers.getContentLengthValue());
  EXPECT_EQ(1UL, headers.size());
  EXPECT_FALSE(headers.empty());
  EXPECT_EQ(1UL, headers.removeContentLength());
  EXPECT_EQ(nullptr, headers.ContentLength());
  EXPECT_EQ(0UL, headers.size());
  EXPECT_TRUE(headers.empty());

  // Add inline and remove by name.
  headers.setContentLength(5);
  EXPECT_EQ("5", headers.getContentLengthValue());
  EXPECT_EQ(1UL, headers.size());
  EXPECT_FALSE(headers.empty());
  EXPECT_EQ(1UL, headers.remove(Headers::get().ContentLength));
  EXPECT_EQ(nullptr, headers.ContentLength());
  EXPECT_EQ(0UL, headers.size());
  EXPECT_TRUE(headers.empty());

  // Try to remove nonexistent headers.
  EXPECT_EQ(0UL, headers.remove(static_key));
  EXPECT_EQ(0UL, headers.remove(Headers::get().ContentLength));
}