TEST(HeaderIsValidTest, HeaderNameContainsUnderscore) {
  EXPECT_FALSE(HeaderUtility::headerNameContainsUnderscore("cookie"));
  EXPECT_FALSE(HeaderUtility::headerNameContainsUnderscore("x-something"));
  EXPECT_TRUE(HeaderUtility::headerNameContainsUnderscore("_cookie"));
  EXPECT_TRUE(HeaderUtility::headerNameContainsUnderscore("cookie_"));
  EXPECT_TRUE(HeaderUtility::headerNameContainsUnderscore("x_something"));
}