TEST(MatchHeadersTest, MustMatchAllHeaderData) {
  TestRequestHeaderMapImpl matching_headers_1{{"match-header-A", "1"}, {"match-header-B", "2"}};
  TestRequestHeaderMapImpl matching_headers_2{
      {"match-header-A", "3"}, {"match-header-B", "4"}, {"match-header-C", "5"}};
  TestRequestHeaderMapImpl unmatching_headers_1{{"match-header-A", "6"}};
  TestRequestHeaderMapImpl unmatching_headers_2{{"match-header-B", "7"}};
  TestRequestHeaderMapImpl unmatching_headers_3{{"match-header-A", "8"}, {"match-header-C", "9"}};
  TestRequestHeaderMapImpl unmatching_headers_4{{"match-header-C", "10"}, {"match-header-D", "11"}};

  const std::string yamlA = R"EOF(
name: match-header-A
  )EOF";

  const std::string yamlB = R"EOF(
name: match-header-B
  )EOF";

  std::vector<HeaderUtility::HeaderDataPtr> header_data;
  header_data.push_back(
      std::make_unique<HeaderUtility::HeaderData>(parseHeaderMatcherFromYaml(yamlA)));
  header_data.push_back(
      std::make_unique<HeaderUtility::HeaderData>(parseHeaderMatcherFromYaml(yamlB)));
  EXPECT_TRUE(HeaderUtility::matchHeaders(matching_headers_1, header_data));
  EXPECT_TRUE(HeaderUtility::matchHeaders(matching_headers_2, header_data));
  EXPECT_FALSE(HeaderUtility::matchHeaders(unmatching_headers_1, header_data));
  EXPECT_FALSE(HeaderUtility::matchHeaders(unmatching_headers_2, header_data));
  EXPECT_FALSE(HeaderUtility::matchHeaders(unmatching_headers_3, header_data));
  EXPECT_FALSE(HeaderUtility::matchHeaders(unmatching_headers_4, header_data));
}