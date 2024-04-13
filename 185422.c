TEST_F(ExtractorTest, TestPrefixHeaderFlexibleMatch1) {
  auto headers =
      TestRequestHeaderMapImpl{{"prefix-header", "preamble CCCDDD=jwt_token,extra=more"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 1);

  // Match issuer 7 with map key as: prefix-header + 'CCCDDD'
  EXPECT_TRUE(tokens[0]->isIssuerSpecified("issuer7"));
  EXPECT_EQ(tokens[0]->token(), "jwt_token");
}