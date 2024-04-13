TEST_F(HeaderToMetadataTest, ConfigIsCached) {
  // Global config is empty.
  initializeFilter("{}");
  Http::TestRequestHeaderMapImpl incoming_headers{{"X-VERSION", "0xdeadbeef"}};
  std::map<std::string, std::string> expected = {{"version", "0xdeadbeef"}};

  // Setup per route config.
  envoy::extensions::filters::http::header_to_metadata::v3::Config config_proto;
  TestUtility::loadFromYaml(request_config_yaml, config_proto);
  Config per_route_config(config_proto, true);
  EXPECT_CALL(decoder_callbacks_.route_->route_entry_.virtual_host_,
              perFilterConfig(HttpFilterNames::get().HeaderToMetadata))
      .WillOnce(Return(&per_route_config));

  EXPECT_TRUE(getConfig()->doRequest());
  EXPECT_TRUE(getConfig()->doRequest());
}