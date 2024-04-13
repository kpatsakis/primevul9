TEST(HeaderMapImplTest, MoveIntoInline) {
  TestRequestHeaderMapImpl headers;
  HeaderString key;
  key.setCopy(Headers::get().EnvoyRetryOn.get());
  HeaderString value;
  value.setCopy("hello");
  headers.addViaMove(std::move(key), std::move(value));
  EXPECT_EQ("x-envoy-retry-on", headers.EnvoyRetryOn()->key().getStringView());
  EXPECT_EQ("hello", headers.getEnvoyRetryOnValue());

  HeaderString key2;
  key2.setCopy(Headers::get().EnvoyRetryOn.get());
  HeaderString value2;
  value2.setCopy("there");
  headers.addViaMove(std::move(key2), std::move(value2));
  EXPECT_EQ("x-envoy-retry-on", headers.EnvoyRetryOn()->key().getStringView());
  EXPECT_EQ("hello,there", headers.getEnvoyRetryOnValue());
}