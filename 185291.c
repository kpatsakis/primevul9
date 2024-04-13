TEST(HeaderIsValidTest, IsConnect) {
  EXPECT_TRUE(HeaderUtility::isConnect(Http::TestRequestHeaderMapImpl{{":method", "CONNECT"}}));
  EXPECT_FALSE(HeaderUtility::isConnect(Http::TestRequestHeaderMapImpl{{":method", "GET"}}));
  EXPECT_FALSE(HeaderUtility::isConnect(Http::TestRequestHeaderMapImpl{}));
}