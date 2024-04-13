TEST_F(ExtractorTest, TestPrefixHeaderNotMatch) {
  auto headers = TestRequestHeaderMapImpl{{"prefix-header", "jwt_token"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 0);
}