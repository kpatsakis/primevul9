TEST(Context, FilterStateAttributes) {
  StreamInfo::FilterStateImpl filter_state(StreamInfo::FilterState::LifeSpan::FilterChain);
  FilterStateWrapper wrapper(filter_state);
  ProtobufWkt::Arena arena;
  wrapper.Produce(&arena);

  const std::string key = "filter_state_key";
  const std::string serialized = "filter_state_value";
  const std::string missing = "missing_key";

  auto accessor = std::make_shared<Envoy::Router::StringAccessorImpl>(serialized);
  filter_state.setData(key, accessor, StreamInfo::FilterState::StateType::ReadOnly);

  {
    auto value = wrapper[CelValue::CreateStringView(missing)];
    EXPECT_FALSE(value.has_value());
  }

  {
    auto value = wrapper[CelValue::CreateStringView(key)];
    EXPECT_TRUE(value.has_value());
    EXPECT_TRUE(value.value().IsBytes());
    EXPECT_EQ(serialized, value.value().BytesOrDie().value());
  }
}