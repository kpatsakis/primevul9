TEST(MatchHeadersTest, MayMatchOneOrMoreRequestHeader) {
  TestRequestHeaderMapImpl headers{{"some-header", "a"}, {"other-header", "b"}};

  const std::string yaml = R"EOF(
name: match-header
regex_match: (a|b)
  )EOF";

  std::vector<HeaderUtility::HeaderDataPtr> header_data;
  header_data.push_back(
      std::make_unique<HeaderUtility::HeaderData>(parseHeaderMatcherFromYaml(yaml)));
  EXPECT_FALSE(HeaderUtility::matchHeaders(headers, header_data));

  headers.addCopy("match-header", "a");
  // With a single "match-header" this regex will match.
  EXPECT_TRUE(HeaderUtility::matchHeaders(headers, header_data));

  headers.addCopy("match-header", "b");
  // With two "match-header" we now logically have "a,b" as the value, so the regex will not match.
  EXPECT_FALSE(HeaderUtility::matchHeaders(headers, header_data));

  header_data[0] = std::make_unique<HeaderUtility::HeaderData>(parseHeaderMatcherFromYaml(R"EOF(
name: match-header
exact_match: a,b
  )EOF"));
  // Make sure that an exact match on "a,b" does in fact work.
  EXPECT_TRUE(HeaderUtility::matchHeaders(headers, header_data));

  TestScopedRuntime runtime;
  Runtime::LoaderSingleton::getExisting()->mergeValues(
      {{"envoy.reloadable_features.http_match_on_all_headers", "false"}});
  // Flipping runtime to false should make "a,b" no longer match because we will match on the first
  // header only.
  EXPECT_FALSE(HeaderUtility::matchHeaders(headers, header_data));

  header_data[0] = std::make_unique<HeaderUtility::HeaderData>(parseHeaderMatcherFromYaml(R"EOF(
name: match-header
exact_match: a
  )EOF"));
  // With runtime off, exact match on "a" should pass.
  EXPECT_TRUE(HeaderUtility::matchHeaders(headers, header_data));
}