TEST(HeaderDataConstructorTest, RangeMatchSpecifier) {
  const std::string yaml = R"EOF(
name: test-header
range_match:
  start: 0
  end: -10
  )EOF";

  HeaderUtility::HeaderData header_data =
      HeaderUtility::HeaderData(parseHeaderMatcherFromYaml(yaml));

  EXPECT_EQ("test-header", header_data.name_.get());
  EXPECT_EQ(HeaderUtility::HeaderMatchType::Range, header_data.header_match_type_);
  EXPECT_EQ("", header_data.value_);
  EXPECT_EQ(0, header_data.range_.start());
  EXPECT_EQ(-10, header_data.range_.end());
}