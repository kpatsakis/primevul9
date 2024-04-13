TEST_F(HeaderToMetadataTest, PerRouteOverride) {
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

  EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(incoming_headers, false));
  Http::MetadataMap metadata_map{{"metadata", "metadata"}};
  EXPECT_EQ(Http::FilterMetadataStatus::Continue, filter_->decodeMetadata(metadata_map));
  Buffer::OwnedImpl data("data");
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter_->decodeData(data, false));
  Http::TestRequestTrailerMapImpl incoming_trailers;
  EXPECT_EQ(Http::FilterTrailersStatus::Continue, filter_->decodeTrailers(incoming_trailers));
  filter_->onDestroy();
}