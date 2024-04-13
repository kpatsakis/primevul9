TEST(MatchHeadersTest, HeaderContainsInverseMatch) {
  TestRequestHeaderMapImpl matching_headers{{"match-header", "123onevalue456"}};
  TestRequestHeaderMapImpl unmatching_headers{{"match-header", "123anothervalue456"}};

  const std::string yaml = R"EOF(
name: match-header
contains_match: onevalue
invert_match: true
  )EOF";

  std::vector<HeaderUtility::HeaderDataPtr> header_data;
  header_data.push_back(
      std::make_unique<HeaderUtility::HeaderData>(parseHeaderMatcherFromYaml(yaml)));
  EXPECT_TRUE(HeaderUtility::matchHeaders(unmatching_headers, header_data));
  EXPECT_FALSE(HeaderUtility::matchHeaders(matching_headers, header_data));
}