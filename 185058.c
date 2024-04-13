TEST_F(HeaderToMetadataTest, StringTypeInBase64UrlTest) {
  const std::string response_config_yaml = R"EOF(
response_rules:
  - header: x-authenticated
    on_header_present:
      key: auth
      type: STRING
      encode: BASE64
)EOF";
  initializeFilter(response_config_yaml);
  std::string data = "Non-ascii-characters";
  const auto encoded = Base64::encode(data.c_str(), data.size());
  Http::TestResponseHeaderMapImpl incoming_headers{{"x-authenticated", encoded}};
  std::map<std::string, std::string> expected = {{"auth", data}};
  Http::TestResponseHeaderMapImpl empty_headers;

  EXPECT_CALL(encoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_,
              setDynamicMetadata(HttpFilterNames::get().HeaderToMetadata, MapEq(expected)));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(incoming_headers, false));
}