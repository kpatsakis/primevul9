TEST_P(RBACIntegrationTest, LogConnectionAllow) {
  config_helper_.addFilter(RBAC_CONFIG_WITH_LOG_ACTION);
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeRequestWithBody(
      Http::TestRequestHeaderMapImpl{
          {":method", "POST"},
          {":path", "/"},
          {":scheme", "http"},
          {":authority", "host"},
          {"x-forwarded-for", "10.0.0.1"},
      },
      1024);
  waitForNextUpstreamRequest();
  upstream_request_->encodeHeaders(Http::TestResponseHeaderMapImpl{{":status", "200"}}, true);

  response->waitForEndStream();
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("200", response->headers().getStatusValue());
}