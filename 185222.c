TEST(HeaderDataConstructorTest, InvertMatchSpecifier) {
  const std::string yaml = R"EOF(
name: test-header
exact_match: value
invert_match: true
)EOF";

  HeaderUtility::HeaderData header_data =
      HeaderUtility::HeaderData(parseHeaderMatcherFromYaml(yaml));

  EXPECT_EQ("test-header", header_data.name_.get());
  EXPECT_EQ(HeaderUtility::HeaderMatchType::Value, header_data.header_match_type_);
  EXPECT_EQ("value", header_data.value_);
  EXPECT_EQ(true, header_data.invert_match_);
}