TEST(Context, EmptyHeadersAttributes) {
  Protobuf::Arena arena;
  HeadersWrapper<Http::RequestHeaderMap> headers(arena, nullptr);
  auto header = headers[CelValue::CreateStringView(Referer)];
  EXPECT_FALSE(header.has_value());
  EXPECT_EQ(0, headers.size());
  EXPECT_TRUE(headers.empty());
}