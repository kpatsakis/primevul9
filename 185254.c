TEST_F(HeaderToMetadataTest, MultipleHeadersMatch) {
  const std::string python_yaml = R"EOF(
request_rules:
  - header: x-version
    on_header_present:
      key: version
      metadata_namespace: envoy.lb
      type: STRING
  - header: x-python-version
    on_header_present:
      key: python_version
      metadata_namespace: envoy.lb
      type: STRING
)EOF";
  initializeFilter(python_yaml);
  Http::TestRequestHeaderMapImpl incoming_headers{
      {"X-VERSION", "v4.0"},
      {"X-PYTHON-VERSION", "3.7"},
      {"X-IGNORE", "nothing"},
  };
  std::map<std::string, std::string> expected = {{"version", "v4.0"}, {"python_version", "3.7"}};

  EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(incoming_headers, false));
}