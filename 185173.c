TEST_F(HeaderToMetadataTest, OnMissingWhenHeaderIsPresent) {
  const std::string config = R"EOF(
request_rules:
  - header: x-version
    on_header_missing:
      metadata_namespace: envoy.lb
      key: version
      value: some_value
      type: STRING
)EOF";
  initializeFilter(config);
  Http::TestRequestHeaderMapImpl headers{{"x-version", ""}};

  EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_, setDynamicMetadata(_, _)).Times(0);
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(headers, false));
}