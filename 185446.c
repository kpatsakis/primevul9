TEST(HeaderIsValidTest, IsConnectResponse) {
  RequestHeaderMapPtr connect_request{new TestRequestHeaderMapImpl{{":method", "CONNECT"}}};
  RequestHeaderMapPtr get_request{new TestRequestHeaderMapImpl{{":method", "GET"}}};
  TestResponseHeaderMapImpl success_response{{":status", "200"}};
  TestResponseHeaderMapImpl failure_response{{":status", "500"}};

  EXPECT_TRUE(HeaderUtility::isConnectResponse(connect_request.get(), success_response));
  EXPECT_FALSE(HeaderUtility::isConnectResponse(connect_request.get(), failure_response));
  EXPECT_FALSE(HeaderUtility::isConnectResponse(nullptr, success_response));
  EXPECT_FALSE(HeaderUtility::isConnectResponse(get_request.get(), success_response));
}