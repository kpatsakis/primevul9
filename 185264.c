TEST(HeaderMapImplTest, AddCopy) {
  TestRequestHeaderMapImpl headers;

  // Start with a string value.
  std::unique_ptr<LowerCaseString> lcKeyPtr(new LowerCaseString("hello"));
  headers.addCopy(*lcKeyPtr, "world");

  const HeaderString& value = headers.get(*lcKeyPtr)->value();

  EXPECT_EQ("world", value.getStringView());
  EXPECT_EQ(5UL, value.size());

  lcKeyPtr.reset();

  const HeaderString& value2 = headers.get(LowerCaseString("hello"))->value();

  EXPECT_EQ("world", value2.getStringView());
  EXPECT_EQ(5UL, value2.size());
  EXPECT_EQ(value.getStringView(), value2.getStringView());
  EXPECT_EQ(1UL, headers.size());

  // Repeat with an int value.
  //
  // addReferenceKey and addCopy can both add multiple instances of a
  // given header, so we need to delete the old "hello" header.
  // Test that removing will return 0 byte size.
  EXPECT_EQ(1UL, headers.remove(LowerCaseString("hello")));
  EXPECT_EQ(headers.byteSize(), 0);

  // Build "hello" with string concatenation to make it unlikely that the
  // compiler is just reusing the same string constant for everything.
  lcKeyPtr = std::make_unique<LowerCaseString>(std::string("he") + "llo");
  EXPECT_STREQ("hello", lcKeyPtr->get().c_str());

  headers.addCopy(*lcKeyPtr, 42);

  const HeaderString& value3 = headers.get(*lcKeyPtr)->value();

  EXPECT_EQ("42", value3.getStringView());
  EXPECT_EQ(2UL, value3.size());

  lcKeyPtr.reset();

  const HeaderString& value4 = headers.get(LowerCaseString("hello"))->value();

  EXPECT_EQ("42", value4.getStringView());
  EXPECT_EQ(2UL, value4.size());
  EXPECT_EQ(1UL, headers.size());

  // Here, again, we'll build yet another key string.
  LowerCaseString lcKey3(std::string("he") + "ll" + "o");
  EXPECT_STREQ("hello", lcKey3.get().c_str());

  EXPECT_EQ("42", headers.get(lcKey3)->value().getStringView());
  EXPECT_EQ(2UL, headers.get(lcKey3)->value().size());

  LowerCaseString envoy_retry_on("x-envoy-retry-on");
  headers.addCopy(envoy_retry_on, "max-age=1345");
  EXPECT_EQ("max-age=1345", headers.get(envoy_retry_on)->value().getStringView());
  EXPECT_EQ("max-age=1345", headers.getEnvoyRetryOnValue());
  headers.addCopy(envoy_retry_on, "public");
  EXPECT_EQ("max-age=1345,public", headers.get(envoy_retry_on)->value().getStringView());
  headers.addCopy(envoy_retry_on, "");
  EXPECT_EQ("max-age=1345,public", headers.get(envoy_retry_on)->value().getStringView());
  headers.addCopy(envoy_retry_on, 123);
  EXPECT_EQ("max-age=1345,public,123", headers.get(envoy_retry_on)->value().getStringView());
  headers.addCopy(envoy_retry_on, std::numeric_limits<uint64_t>::max());
  EXPECT_EQ("max-age=1345,public,123,18446744073709551615",
            headers.get(envoy_retry_on)->value().getStringView());
}