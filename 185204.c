TEST_F(HeaderToMetadataTest, HeaderRemovedTest) {
  const std::string response_config_yaml = R"EOF(
response_rules:
  - header: x-authenticated
    on_header_present:
      key: auth
      type: STRING
    remove: true
)EOF";
  initializeFilter(response_config_yaml);
  Http::TestResponseHeaderMapImpl incoming_headers{{"x-authenticated", "1"}};
  std::map<std::string, std::string> expected = {{"auth", "1"}};
  Http::TestResponseHeaderMapImpl empty_headers;

  EXPECT_CALL(encoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
  EXPECT_CALL(req_info_,
              setDynamicMetadata(HttpFilterNames::get().HeaderToMetadata, MapEq(expected)));
  Http::TestResponseHeaderMapImpl continue_response{{":status", "100"}};
  EXPECT_EQ(Http::FilterHeadersStatus::Continue,
            filter_->encode100ContinueHeaders(continue_response));
  EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->encodeHeaders(incoming_headers, false));
  EXPECT_EQ(empty_headers, incoming_headers);
  Http::MetadataMap metadata_map{{"metadata", "metadata"}};
  EXPECT_EQ(Http::FilterMetadataStatus::Continue, filter_->encodeMetadata(metadata_map));
  Buffer::OwnedImpl data("data");
  EXPECT_EQ(Http::FilterDataStatus::Continue, filter_->encodeData(data, false));
  Http::TestResponseTrailerMapImpl incoming_trailers;
  EXPECT_EQ(Http::FilterTrailersStatus::Continue, filter_->encodeTrailers(incoming_trailers));
}