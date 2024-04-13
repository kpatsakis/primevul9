TEST(HeaderIsValidTest, ValidHeaderValuesAreAccepted) {
  EXPECT_TRUE(HeaderUtility::headerValueIsValid("some-value"));
  EXPECT_TRUE(HeaderUtility::headerValueIsValid("Some Other Value"));
}