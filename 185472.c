TEST_F(ExtractorTest, TestExtractParam) {
  auto headers = TestRequestHeaderMapImpl{
      {":path", "/path?token_param=token3&access_token=token4"},
      {"token-header", "token2"},
      {"authorization", "Bearer token1"},
      {"prefix-header", "AAAtoken5"},
  };
  JwtProvider provider;
  provider.set_issuer("foo");
  auto extractor = Extractor::create(provider);
  auto tokens = extractor->extract(headers);
  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0]->token(), "token1");
  EXPECT_EQ(tokens[1]->token(), "token4");
  auto header = provider.add_from_headers();
  header->set_name("prefix-header");
  header->set_value_prefix("AAA");
  provider.add_from_params("token_param");
  extractor = Extractor::create(provider);
  tokens = extractor->extract(headers);
  EXPECT_EQ(tokens.size(), 2);
  EXPECT_EQ(tokens[0]->token(), "token5");
  EXPECT_EQ(tokens[1]->token(), "token3");
}