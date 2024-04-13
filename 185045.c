TEST_F(HeaderToMetadataTest, HeaderValueTooLong) {
  initializeFilter(request_config_yaml);
  auto length = Envoy::Extensions::HttpFilters::HeaderToMetadataFilter::MAX_HEADER_VALUE_LEN + 1;
  Http::TestRequestHeaderMapImpl incoming_headers{{"X-VERSION", std::string(length, 'x')}};

  EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_, setDynamicMetadata(_, _)).Times(0);
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(incoming_headers, false));
}