TEST(HeaderAddTest, HeaderAdd) {
  TestRequestHeaderMapImpl headers{{"myheader1", "123value"}};
  TestRequestHeaderMapImpl headers_to_add{{"myheader2", "456value"}};

  HeaderUtility::addHeaders(headers, headers_to_add);

  headers_to_add.iterate([&headers](const Http::HeaderEntry& entry) -> Http::HeaderMap::Iterate {
    Http::LowerCaseString lower_key{std::string(entry.key().getStringView())};
    EXPECT_EQ(entry.value().getStringView(), headers.get(lower_key)->value().getStringView());
    return Http::HeaderMap::Iterate::Continue;
  });
}