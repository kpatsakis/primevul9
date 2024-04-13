TEST_F(HeaderToMetadataTest, ProtobufValueTypeInBadBase64UrlTest) {
  const std::string response_config_yaml = R"EOF(
response_rules:
  - header: x-authenticated
    on_header_present:
      key: auth
      type: PROTOBUF_VALUE
      encode: BASE64
)EOF";
  initializeFilter(response_config_yaml);
  Http::TestResponseHeaderMapImpl incoming_headers{{"x-authenticated", "invalid"}};

  EXPECT_CALL(encoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_, setDynamicMetadata(_, _)).Times(0);
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(incoming_headers, false));
}