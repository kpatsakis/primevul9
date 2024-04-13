TEST_F(ExtractorTest, TestCustomParamToken) {
  auto headers = TestRequestHeaderMapImpl{{":path", "/path?token_param=jwt_token"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 1);

  // Both issuer3 and issuer4 have specified this custom query location.
  EXPECT_EQ(tokens[0]->token(), "jwt_token");
  EXPECT_TRUE(tokens[0]->isIssuerSpecified("issuer3"));
  EXPECT_TRUE(tokens[0]->isIssuerSpecified("issuer4"));

  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer1"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer2"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer5"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("unknown_issuer"));

  tokens[0]->removeJwt(headers);
}