TEST(HeaderDataConstructorTest, PresentMatchSpecifier) {
  const std::string yaml = R"EOF(
name: test-header
present_match: true
  )EOF";

  HeaderUtility::HeaderData header_data =
      HeaderUtility::HeaderData(parseHeaderMatcherFromYaml(yaml));

  EXPECT_EQ("test-header", header_data.name_.get());
  EXPECT_EQ(HeaderUtility::HeaderMatchType::Present, header_data.header_match_type_);
  EXPECT_EQ("", header_data.value_);
}