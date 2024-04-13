TEST(HeaderMapImplTest, TestHeaderList) {
  std::array<std::string, 2> keys{Headers::get().Path.get(), "hello"};
  std::array<std::string, 2> values{"/", "world"};

  auto headers = TestRequestHeaderMapImpl({{keys[0], values[0]}, {keys[1], values[1]}});
  HeaderListView header_list(headers);
  auto to_string_views =
      [](const HeaderListView::HeaderStringRefs& strs) -> std::vector<absl::string_view> {
    std::vector<absl::string_view> str_views(strs.size());
    std::transform(strs.begin(), strs.end(), str_views.begin(),
                   [](auto value) -> absl::string_view { return value.get().getStringView(); });
    return str_views;
  };

  EXPECT_THAT(to_string_views(header_list.keys()), ElementsAre(":path", "hello"));
  EXPECT_THAT(to_string_views(header_list.values()), ElementsAre("/", "world"));
}