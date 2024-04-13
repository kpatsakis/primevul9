TEST_F(HeaderToMetadataTest, EmptyHeaderValue) {
  initializeFilter(request_config_yaml);
  Http::TestRequestHeaderMapImpl incoming_headers{{"X-VERSION", ""}};

  EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_, setDynamicMetadata(_, _)).Times(0);
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(incoming_headers, false));
}