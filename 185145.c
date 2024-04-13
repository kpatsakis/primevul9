TEST_P(RBACIntegrationTest, DeniedWithPrefixRule) {
  config_helper_.addConfigModifier(
      [](envoy::extensions::filters::network::http_connection_manager::v3::HttpConnectionManager&
             cfg) { cfg.mutable_normalize_path()->set_value(false); });
  config_helper_.addFilter(RBAC_CONFIG_WITH_PREFIX_MATCH);
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeRequestWithBody(
      Http::TestRequestHeaderMapImpl{
          {":method", "POST"},
          {":path", "/foo/../bar"},
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