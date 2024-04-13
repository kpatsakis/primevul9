TEST(MatchHeadersTest, HeaderPresence) {
  TestRequestHeaderMapImpl matching_headers{{"match-header", "value"}};
  TestRequestHeaderMapImpl unmatching_headers{{"other-header", "value"}};
  const std::string yaml = R"EOF(
name: match-header
  )EOF";

  std::vector<HeaderUtility::HeaderDataPtr> header_data;
  header_data.push_back(
      std::make_unique<HeaderUtility::HeaderData>(parseHeaderMatcherFromYaml(yaml)));
  EXPECT_TRUE(HeaderUtility::matchHeaders(matching_headers, header_data));
  EXPECT_FALSE(HeaderUtility::matchHeaders(unmatching_headers, header_data));
}