TEST(Context, InvalidRequest) {
  Http::TestRequestHeaderMapImpl header_map{{"referer", "dogs.com"}};
  Protobuf::Arena arena;
  HeadersWrapper<Http::RequestHeaderMap> headers(arena, &header_map);
  auto header = headers[CelValue::CreateStringView("dogs.com\n")];
  EXPECT_FALSE(header.has_value());
}