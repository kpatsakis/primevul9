TEST_F(ExtractorTest, TestPrefixHeaderMatch) {
  auto headers = TestRequestHeaderMapImpl{{"prefix-header", "AAABBBjwt_token"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 2);

  // Match issuer 5 with map key as: prefix-header + AAA
  EXPECT_TRUE(tokens[0]->isIssuerSpecified("issuer5"));
  EXPECT_EQ(tokens[0]->token(), "BBBjwt_token");

  // Match issuer 6 with map key as: prefix-header + AAABBB which is after AAA
  EXPECT_TRUE(tokens[1]->isIssuerSpecified("issuer6"));
  EXPECT_EQ(tokens[1]->token(), "jwt_token");

  // Test token remove
  tokens[0]->removeJwt(headers);
  EXPECT_FALSE(headers.get(Http::LowerCaseString("prefix-header")));
}