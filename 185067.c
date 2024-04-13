TEST_F(HeaderToMetadataTest, BasicRequestDoubleHeadersTest) {
  initializeFilter(request_config_yaml);
  Http::TestRequestHeaderMapImpl incoming_headers{{"X-VERSION", "foo"}, {"X-VERSION", "bar"}};
  std::map<std::string, std::string> expected = {{"version", "foo,bar"}};

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