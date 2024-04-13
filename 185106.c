TEST_F(ExtractorTest, TestCustomHeaderToken) {
  auto headers = TestRequestHeaderMapImpl{{"token-header", "jwt_token"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 1);

  // Only issuer2 and issuer4 are using "token-header" location
  EXPECT_EQ(tokens[0]->token(), "jwt_token");
  EXPECT_TRUE(tokens[0]->isIssuerSpecified("issuer2"));
  EXPECT_TRUE(tokens[0]->isIssuerSpecified("issuer4"));

  // Other issuers are not allowed from "token-header"
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer1"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer3"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("issuer5"));
  EXPECT_FALSE(tokens[0]->isIssuerSpecified("unknown_issuer"));

  // Test token remove
  tokens[0]->removeJwt(headers);
  EXPECT_FALSE(headers.get(Http::LowerCaseString("token-header")));
}