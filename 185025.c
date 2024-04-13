TEST_F(HeaderToMetadataTest, HeaderNotPresent) {
  const std::string config = R"EOF(
request_rules:
  - header: x-version
    on_header_present:
      metadata_namespace: envoy.lb
      key: version
      type: STRING
)EOF";
  initializeFilter(config);
  Http::TestRequestHeaderMapImpl incoming_headers;

  EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_, setDynamicMetadata(_, _)).Times(0);
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(incoming_headers, false));
}