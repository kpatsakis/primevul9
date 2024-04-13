TEST_P(RBACIntegrationTest, PathIgnoreCase) {
  config_helper_.addFilter(RBAC_CONFIG_WITH_PATH_IGNORE_CASE_MATCH);
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  const std::vector<std::string> paths{"/ignore_case", "/IGNORE_CASE", "/ignore_CASE"};

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