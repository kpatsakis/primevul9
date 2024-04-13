TEST_F(HeaderToMetadataTest, PerRouteEmtpyRules) {
  envoy::extensions::filters::http::header_to_metadata::v3::Config config_proto;
  EXPECT_THROW(std::make_shared<Config>(config_proto, true), EnvoyException);
}