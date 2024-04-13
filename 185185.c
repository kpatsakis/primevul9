TEST(MatchHeadersTest, HeaderPrefixMatch) {
  TestRequestHeaderMapImpl matching_headers{{"match-header", "value123"}};
  TestRequestHeaderMapImpl unmatching_headers{{"match-header", "123value"}};

  const std::string yaml = R"EOF(
name: match-header
prefix_match: value
  )EOF";

  std::vector<HeaderUtility::HeaderDataPtr> header_data;
  header_data.push_back(
      std::make_unique<HeaderUtility::HeaderData>(parseHeaderMatcherFromYaml(yaml)));
  EXPECT_TRUE(HeaderUtility::matchHeaders(matching_headers, header_data));
  EXPECT_FALSE(HeaderUtility::matchHeaders(unmatching_headers, header_data));
}