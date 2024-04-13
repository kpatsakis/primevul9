TEST_F(ExtractorTest, TestNoToken) {
  auto headers = TestRequestHeaderMapImpl{};
  auto tokens = extractor_->extract(headers);
  EXPECT_EQ(tokens.size(), 0);
}