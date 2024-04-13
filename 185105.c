TEST_F(HeaderToMetadataTest, RegexSubstitution) {
  const std::string config = R"EOF(
request_rules:
  - header: :path
    on_header_present:
      metadata_namespace: envoy.lb
      key: cluster
      regex_value_rewrite:
        pattern:
          google_re2: {}
          regex: "^/(cluster[\\d\\w-]+)/?.*$"
        substitution: "\\1"
)EOF";
  initializeFilter(config);

  // Match with additional path elements.
  {
    Http::TestRequestHeaderMapImpl headers{{":path", "/cluster-prod-001/x/y"}};
    std::map<std::string, std::string> expected = {{"cluster", "cluster-prod-001"}};

    EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
    EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
    EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(headers, false));
  }

  // Match with no additional path elements.
  {
    Http::TestRequestHeaderMapImpl headers{{":path", "/cluster-prod-001"}};
    std::map<std::string, std::string> expected = {{"cluster", "cluster-prod-001"}};

    EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
    EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
    EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(headers, false));
  }

  // No match.
  {
    Http::TestRequestHeaderMapImpl headers{{":path", "/foo"}};
    std::map<std::string, std::string> expected = {{"cluster", "/foo"}};

    EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
    EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
    EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(headers, false));
  }

  // No match with additional path elements.
  {
    Http::TestRequestHeaderMapImpl headers{{":path", "/foo/bar?x=2"}};
    std::map<std::string, std::string> expected = {{"cluster", "/foo/bar?x=2"}};

    EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
    EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
    EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(headers, false));
  }
}