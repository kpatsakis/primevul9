TEST_P(RBACIntegrationTest, DeniedHeadReply) {
  config_helper_.addFilter(RBAC_CONFIG);
  initialize();

  codec_client_ = makeHttpConnection(lookupPort("http"));

  auto response = codec_client_->makeRequestWithBody(
      Http::TestRequestHeaderMapImpl{
          {":method", "HEAD"},
          {":path", "/"},
          {":scheme", "http"},
          {":authority", "host"},
          {"x-forwarded-for", "10.0.0.1"},
      },
      1024);
  response->waitForEndStream();
  ASSERT_TRUE(response->complete());
  EXPECT_EQ("403", response->headers().getStatusValue());
  ASSERT_TRUE(response->headers().ContentLength());
  EXPECT_NE("0", response->headers().getContentLengthValue());
  EXPECT_THAT(response->body(), ::testing::IsEmpty());
}