TEST_P(RBACIntegrationTest, Denied) {
  useAccessLog("%RESPONSE_CODE_DETAILS%");
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
  response->waitForEndStream();
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("403", response->headers().getStatusValue());
  EXPECT_THAT(waitForAccessLog(access_log_name_),
              testing::HasSubstr("rbac_access_denied_matched_policy[none]"));
}