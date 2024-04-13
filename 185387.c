TEST_P(RBACIntegrationTest, RouteOverride) {
  config_helper_.addConfigModifier(
      [](envoy::extensions::filters::network::http_connection_manager::v3::HttpConnectionManager&
             cfg) {
        envoy::extensions::filters::http::rbac::v3::RBACPerRoute per_route_config;
        TestUtility::loadFromJson("{}", per_route_config);

        auto* config = cfg.mutable_route_config()
                           ->mutable_virtual_hosts()
                           ->Mutable(0)
                           ->mutable_typed_per_filter_config();

        (*config)[Extensions::HttpFilters::HttpFilterNames::get().Rbac].PackFrom(per_route_config);
      });
  config_helper_.addFilter(RBAC_CONFIG);

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