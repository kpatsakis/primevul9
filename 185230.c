TEST_F(ExtractorTest, TestDefaultHeaderLocation) {
  auto headers = TestRequestHeaderMapImpl{{"Authorization", "Bearer jwt_token"}};
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

  // Test token remove
  tokens[0]->removeJwt(headers);
  EXPECT_FALSE(headers.has(Http::CustomHeaders::get().Authorization));
}