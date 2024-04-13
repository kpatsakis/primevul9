TEST_F(ExtractorTest, TestPrefixHeaderFlexibleMatch3) {
  auto headers = TestRequestHeaderMapImpl{
      {"prefix-header", "creds={\"authLevel\": \"20\", \"CCCDDD\": \"and0X3Rva2Vu\"}"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 2);

  // Match issuer 8 with map key as: prefix-header + '"CCCDDD"'
  EXPECT_TRUE(tokens[0]->isIssuerSpecified("issuer8"));
  EXPECT_EQ(tokens[0]->token(), "and0X3Rva2Vu");

  // Match issuer 7 with map key as: prefix-header + 'CCCDDD'
  EXPECT_TRUE(tokens[1]->isIssuerSpecified("issuer7"));
  EXPECT_EQ(tokens[1]->token(), "and0X3Rva2Vu");
}