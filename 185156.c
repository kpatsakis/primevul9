TEST(HeaderDataConstructorTest, SuffixMatchSpecifier) {
  const std::string yaml = R"EOF(
name: test-header
suffix_match: value
  )EOF";

  HeaderUtility::HeaderData header_data =
      HeaderUtility::HeaderData(parseHeaderMatcherFromYaml(yaml));

  EXPECT_EQ("test-header", header_data.name_.get());
  EXPECT_EQ(HeaderUtility::HeaderMatchType::Suffix, header_data.header_match_type_);
  EXPECT_EQ("value", header_data.value_);
}