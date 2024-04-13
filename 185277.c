TEST_F(ExtractorTest, TestWrongHeaderToken) {
  auto headers = TestRequestHeaderMapImpl{{"wrong-token-header", "jwt_token"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 0);
}