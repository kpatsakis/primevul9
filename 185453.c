TEST(GetAllOfHeaderAsStringTest, All) {
  const LowerCaseString test_header("test");
  {
    TestRequestHeaderMapImpl headers;
    const auto ret = HeaderUtility::getAllOfHeaderAsString(headers, test_header);
    EXPECT_FALSE(ret.result().has_value());
    EXPECT_TRUE(ret.backingString().empty());
  }
  {
    TestRequestHeaderMapImpl headers{{"test", "foo"}};
    const auto ret = HeaderUtility::getAllOfHeaderAsString(headers, test_header);
    EXPECT_EQ("foo", ret.result().value());
    EXPECT_TRUE(ret.backingString().empty());
  }
  {
    TestRequestHeaderMapImpl headers{{"test", "foo"}, {"test", "bar"}};
    const auto ret = HeaderUtility::getAllOfHeaderAsString(headers, test_header);
    EXPECT_EQ("foo,bar", ret.result().value());
    EXPECT_EQ("foo,bar", ret.backingString());
  }
  {
    TestRequestHeaderMapImpl headers{{"test", ""}, {"test", "bar"}};
    const auto ret = HeaderUtility::getAllOfHeaderAsString(headers, test_header);
    EXPECT_EQ(",bar", ret.result().value());
    EXPECT_EQ(",bar", ret.backingString());
  }
  {
    TestRequestHeaderMapImpl headers{{"test", ""}, {"test", ""}};
    const auto ret = HeaderUtility::getAllOfHeaderAsString(headers, test_header);
    EXPECT_EQ(",", ret.result().value());
    EXPECT_EQ(",", ret.backingString());
  }
  {
    TestRequestHeaderMapImpl headers{
        {"test", "a"}, {"test", "b"}, {"test", "c"}, {"test", ""}, {"test", ""}};
    const auto ret = HeaderUtility::getAllOfHeaderAsString(headers, test_header);
    EXPECT_EQ("a,b,c,,", ret.result().value());
    EXPECT_EQ("a,b,c,,", ret.backingString());
    // Make sure copying the return value works correctly.
    const auto ret2 = ret; // NOLINT(performance-unnecessary-copy-initialization)
    EXPECT_EQ(ret2.result(), ret.result());
    EXPECT_EQ(ret2.backingString(), ret.backingString());
    EXPECT_EQ(ret2.result().value().data(), ret2.backingString().data());
    EXPECT_NE(ret2.result().value().data(), ret.backingString().data());
  }
}