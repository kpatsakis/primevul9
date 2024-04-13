TEST_F(HeaderToMetadataTest, NumberTypeTest) {
  const std::string response_config_yaml = R"EOF(
response_rules:
  - header: x-authenticated
    on_header_present:
      key: auth
      type: NUMBER
)EOF";
  initializeFilter(response_config_yaml);
  Http::TestResponseHeaderMapImpl incoming_headers{{"x-authenticated", "1"}};
  std::map<std::string, int> expected = {{"auth", 1}};
  Http::TestResponseHeaderMapImpl empty_headers;

  EXPECT_CALL(encoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_,
              setDynamicMetadata(HttpFilterNames::get().HeaderToMetadata, MapEqNum(expected)));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(incoming_headers, false));
}