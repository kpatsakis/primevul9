TEST(HeaderIsValidTest, InvalidHeaderValuesAreRejected) {
  // ASCII values 1-31 are control characters (with the exception of ASCII
  // values 9, 10, and 13 which are a horizontal tab, line feed, and carriage
  // return, respectively), and are not valid in an HTTP header, per
  // RFC 7230, section 3.2
  for (int i = 0; i < 32; i++) {
    if (i == 9) {
      continue;
    }

    EXPECT_FALSE(HeaderUtility::headerValueIsValid(std::string(1, i)));
  }
}