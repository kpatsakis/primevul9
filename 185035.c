TEST(HeaderDataConstructorTest, ExactMatchSpecifier) {
  const std::string yaml = R"EOF(
name: test-header
exact_match: value
  )EOF";

  HeaderUtility::HeaderData header_data =
      HeaderUtility::HeaderData(parseHeaderMatcherFromYaml(yaml));

  EXPECT_EQ("test-header", header_data.name_.get());
  EXPECT_EQ(HeaderUtility::HeaderMatchType::Value, header_data.header_match_type_);
  EXPECT_EQ("value", header_data.value_);
}