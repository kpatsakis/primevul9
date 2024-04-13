TEST_F(HeaderToMetadataTest, ProtobufValueTypeInBase64UrlTest) {
  const std::string response_config_yaml = R"EOF(
response_rules:
  - header: x-authenticated
    on_header_present:
      key: auth
      type: PROTOBUF_VALUE
      encode: BASE64
)EOF";
  initializeFilter(response_config_yaml);

  ProtobufWkt::Value value;
  auto* s = value.mutable_struct_value();

  ProtobufWkt::Value v;
  v.set_string_value("blafoo");
  (*s->mutable_fields())["k1"] = v;
  v.set_number_value(2019.07);
  (*s->mutable_fields())["k2"] = v;
  v.set_bool_value(true);
  (*s->mutable_fields())["k3"] = v;

  std::string data;
  ASSERT_TRUE(value.SerializeToString(&data));
  const auto encoded = Base64::encode(data.c_str(), data.size());
  Http::TestResponseHeaderMapImpl incoming_headers{{"x-authenticated", encoded}};
  std::map<std::string, ProtobufWkt::Value> expected = {{"auth", value}};

  EXPECT_CALL(encoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_,
              setDynamicMetadata(HttpFilterNames::get().HeaderToMetadata, MapEqValue(expected)));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(incoming_headers, false));
}