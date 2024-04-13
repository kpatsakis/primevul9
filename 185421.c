TEST_F(HeaderToMetadataTest, CookieRegexSubstitution) {
  const std::string config = R"EOF(
request_rules:
  - cookie: foo
    on_header_present:
      metadata_namespace: envoy.lb
      key: cluster
      regex_value_rewrite:
        pattern:
          google_re2: {}
          regex: "^(cluster[\\d\\w-]+)$"
        substitution: "\\1 matched"
)EOF";
  initializeFilter(config);

  // match.
  {
    Http::TestRequestHeaderMapImpl headers{{"cookie", "foo=cluster-prod-001"}};
    std::map<std::string, std::string> expected = {{"cluster", "cluster-prod-001 matched"}};

    EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
    EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
    EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(headers, false));
  }

  // No match.
  {
    Http::TestRequestHeaderMapImpl headers{{"cookie", "foo=cluster"}};
    std::map<std::string, std::string> expected = {{"cluster", "cluster"}};

    EXPECT_CALL(decoder_callbacks_, streamInfo()).WillRepeatedly(ReturnRef(req_info_));
    EXPECT_CALL(req_info_, setDynamicMetadata("envoy.lb", MapEq(expected)));
    EXPECT_EQ(Http::FilterHeadersStatus::Continue, filter_->decodeHeaders(headers, false));
  }
}