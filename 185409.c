TEST_F(HeaderToMetadataTest, IgnoreHeaderValueUseConstant) {
  const std::string response_config_yaml = R"EOF(
response_rules:
  - header: x-something
    on_header_present:
      key: something
      value: else
      type: STRING
    remove: true
)EOF";
  initializeFilter(response_config_yaml);
  Http::TestResponseHeaderMapImpl incoming_headers{{"x-something", "thing"}};
  std::map<std::string, std::string> expected = {{"something", "else"}};
  Http::TestResponseHeaderMapImpl empty_headers;

  EXPECT_CALL(encoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_,
              setDynamicMetadata(HttpFilterNames::get().HeaderToMetadata, MapEq(expected)));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(incoming_headers, false));
  EXPECT_EQ(empty_headers, incoming_headers);
}