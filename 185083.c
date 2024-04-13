TEST_P(RBACIntegrationTest, HeaderMatchConditionDuplicateHeaderNoMatch) {
  config_helper_.addFilter(fmt::format(RBAC_CONFIG_HEADER_MATCH_CONDITION, "yyy"));
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeRequestWithBody(
      Http::TestRequestHeaderMapImpl{
          {":method", "POST"},
          {":path", "/path"},
          {":scheme", "http"},
          {":authority", "host"},
          {"xxx", "yyy"},
          {"xxx", "zzz"},
      },
      1024);
  response->waitForEndStream();
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("403", response->headers().getStatusValue());
}