TEST_F(HeaderToMetadataTest, NoCookieValue) {
  const std::string config = R"EOF(
request_rules:
  - cookie: foo
    on_header_missing:
      metadata_namespace: envoy.lb
      key: foo
      value: some_value
      type: STRING
)EOF";
  initializeFilter(config);
  Http::TestRequestHeaderMapImpl headers{{"cookie", ""}};
  std::map<std::string, std::string> expected = {{"foo", "some_value"}};

  EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(headers, false));
}