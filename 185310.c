TEST(HeaderMapImplTest, CreateHeaderMapFromIterator) {
  std::vector<std::pair<LowerCaseString, std::string>> iter_headers{
      {LowerCaseString(Headers::get().Path), "/"}, {LowerCaseString("hello"), "world"}};
  auto headers = createHeaderMap<RequestHeaderMapImpl>(iter_headers.cbegin(), iter_headers.cend());
  EXPECT_EQ("/", headers->get(LowerCaseString(":path"))->value().getStringView());
  EXPECT_EQ("world", headers->get(LowerCaseString("hello"))->value().getStringView());
  EXPECT_EQ(nullptr, headers->get(LowerCaseString("foo")));
}