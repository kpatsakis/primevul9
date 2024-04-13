TEST_F(HeaderToMetadataTest, DefaultEndpointsTest) {
  initializeFilter(request_config_yaml);
  Http::TestRequestHeaderMapImpl incoming_headers{{"X-FOO", "bar"}};
  std::map<std::string, std::string> expected = {{"default", "true"}};

  EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(incoming_headers, false));
}