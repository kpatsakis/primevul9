TEST_F(HeaderToMetadataTest, IgnoreCookieValueUseConstant) {
  const std::string response_config_yaml = R"EOF(
response_rules:
  - cookie: meh
    on_header_present:
      key: meh
      value: some_value
      type: STRING
    remove: false
)EOF";
  initializeFilter(response_config_yaml);
  Http::TestResponseHeaderMapImpl incoming_headers{{"cookie", "meh=foo"}};
  std::map<std::string, std::string> expected = {{"meh", "some_value"}};

  EXPECT_CALL(encoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_,
              setDynamicMetadata(HttpFilterNames::get().HeaderToMetadata, MapEq(expected)));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(incoming_headers, false));
}