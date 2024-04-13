TEST_P(RBACIntegrationTest, HeaderMatchConditionDuplicateHeaderMatch) {
  config_helper_.addFilter(fmt::format(RBAC_CONFIG_HEADER_MATCH_CONDITION, "yyy,zzz"));
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
  waitForNextUpstreamRequest();
  upstream_request_->encodeHeaders(Http::TestResponseHeaderMapImpl{{":status", "200"}}, true);

  response->waitForEndStream();
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("200", response->headers().getStatusValue());
}