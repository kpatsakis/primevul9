TEST(HeaderDataConstructorTest, ContainsMatchSpecifier) {
  const std::string yaml = R"EOF(
name: test-header
contains_match: somevalueinside
  )EOF";

  HeaderUtility::HeaderData header_data =
      HeaderUtility::HeaderData(parseHeaderMatcherFromYaml(yaml));

  EXPECT_EQ("test-header", header_data.name_.get());
  EXPECT_EQ(HeaderUtility::HeaderMatchType::Contains, header_data.header_match_type_);
  EXPECT_EQ("somevalueinside", header_data.value_);
}