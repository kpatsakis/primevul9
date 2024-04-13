TEST(HeaderDataConstructorTest, GetAllOfHeader) {
  TestRequestHeaderMapImpl headers{
      {"foo", "val1"}, {"bar", "bar2"}, {"foo", "eep, bar"}, {"foo", ""}};

  std::vector<absl::string_view> foo_out;
  Http::HeaderUtility::getAllOfHeader(headers, "foo", foo_out);
  ASSERT_EQ(foo_out.size(), 3);
  ASSERT_EQ(foo_out[0], "val1");
  ASSERT_EQ(foo_out[1], "eep, bar");
  ASSERT_EQ(foo_out[2], "");

  std::vector<absl::string_view> bar_out;
  Http::HeaderUtility::getAllOfHeader(headers, "bar", bar_out);
  ASSERT_EQ(bar_out.size(), 1);
  ASSERT_EQ(bar_out[0], "bar2");

  std::vector<absl::string_view> eep_out;
  Http::HeaderUtility::getAllOfHeader(headers, "eep", eep_out);
  ASSERT_EQ(eep_out.size(), 0);
}