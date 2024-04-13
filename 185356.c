TEST(HeaderIsValidTest, AuthorityIsValid) {
  EXPECT_TRUE(HeaderUtility::authorityIsValid("strangebutlegal$-%&'"));
  EXPECT_FALSE(HeaderUtility::authorityIsValid("illegal{}"));
}