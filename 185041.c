TEST_F(ExtractorTest, TestWrongParamToken) {
  auto headers = TestRequestHeaderMapImpl{{":path", "/path?wrong_token=jwt_token"}};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 0);
}