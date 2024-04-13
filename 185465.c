TEST_F(ExtractorTest, TestDefaultParamLocation) {
  auto headers = TestRequestHeaderMapImpl{{":path", "/path?access_token=jwt_token"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 1);

  // Only the issue1 is using default header location.
  EXPECT_EQ(tokens[0]->token(), "jwt_token");
  EXPECT_TRUE(tokens[0]->isIssuerSpecified("issuer1"));

  // Other issuers are using custom locations
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer2"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer3"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer4"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer5"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("unknown_issuer"));

  tokens[0]->removeJwt(headers);
}