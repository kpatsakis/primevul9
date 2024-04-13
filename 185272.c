TEST_P(RBACIntegrationTest, PathWithQueryAndFragment) {
  config_helper_.addFilter(RBAC_CONFIG_WITH_PATH_EXACT_MATCH);
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  const std::vector<std::string> paths{"/allow", "/allow?p1=v1&p2=v2", "/allow?p1=v1#seg"};

  for (const auto& path : paths) {
    auto response = codec_client_->makeRequestWithBody(
        Http::TestRequestHeaderMapImpl{
            {":method", "POST"},
            {":path", path},
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
}