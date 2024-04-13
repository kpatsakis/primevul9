TEST_F(ExtractorTest, TestPrefixHeaderFlexibleMatch2) {
  auto headers =
      TestRequestHeaderMapImpl{{"prefix-header", "CCCDDD=\"and0X3Rva2Vu\",comment=\"fish tag\""}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 1);

  // Match issuer 7 with map key as: prefix-header + AAA
  EXPECT_TRUE(tokens[0]->isIssuerSpecified("issuer7"));
  EXPECT_EQ(tokens[0]->token(), "and0X3Rva2Vu");
}