TEST_P(RBACIntegrationTest, DeniedWithDenyAction) {
  useAccessLog("%RESPONSE_CODE_DETAILS%");
  config_helper_.addFilter(RBAC_CONFIG_WITH_DENY_ACTION);
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeRequestWithBody(
      Http::TestRequestHeaderMapImpl{
          {":method", "GET"},
          {":path", "/"},
          {":scheme", "http"},
          {":authority", "host"},
          {"x-forwarded-for", "10.0.0.1"},
      },
      1024);
  response->waitForEndStream();
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("403", response->headers().getStatusValue());
  // Note the whitespace in the policy id is replaced by '_'.
  EXPECT_THAT(waitForAccessLog(access_log_name_),
              testing::HasSubstr("rbac_access_denied_matched_policy[deny_policy]"));
}