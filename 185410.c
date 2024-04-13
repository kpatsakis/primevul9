TEST(PercentEncoding, ShouldCloseConnection) {
  EXPECT_TRUE(HeaderUtility::shouldCloseConnection(Protocol::Http10,
                                                   TestRequestHeaderMapImpl{{"foo", "bar"}}));
  EXPECT_FALSE(HeaderUtility::shouldCloseConnection(
      Protocol::Http10, TestRequestHeaderMapImpl{{"connection", "keep-alive"}}));
  EXPECT_FALSE(HeaderUtility::shouldCloseConnection(
      Protocol::Http10, TestRequestHeaderMapImpl{{"connection", "foo, keep-alive"}}));

  EXPECT_FALSE(HeaderUtility::shouldCloseConnection(Protocol::Http11,
                                                    TestRequestHeaderMapImpl{{"foo", "bar"}}));
  EXPECT_TRUE(HeaderUtility::shouldCloseConnection(
      Protocol::Http11, TestRequestHeaderMapImpl{{"connection", "close"}}));
  EXPECT_TRUE(HeaderUtility::shouldCloseConnection(
      Protocol::Http11, TestRequestHeaderMapImpl{{"connection", "te,close"}}));
  EXPECT_TRUE(HeaderUtility::shouldCloseConnection(
      Protocol::Http11, TestRequestHeaderMapImpl{{"proxy-connection", "close"}}));
  EXPECT_TRUE(HeaderUtility::shouldCloseConnection(
      Protocol::Http11, TestRequestHeaderMapImpl{{"proxy-connection", "foo,close"}}));
}