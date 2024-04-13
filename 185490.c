TEST_F(ExtractorTest, TestDoubleCustomHeaderToken) {
  auto headers = TestRequestHeaderMapImpl{{"token-header", "jwt_token"}, {"token-header", "foo"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 1);
  EXPECT_EQ(tokens[0]->token(), "jwt_token,foo");
}