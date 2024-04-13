TEST_F(LuaStreamInfoWrapperTest, ModifyDuringIterationForDynamicMetadata) {
  const std::string SCRIPT{R"EOF(
    function callMe(object)
      object:dynamicMetadata():set("envoy.lb", "hello", "world")
      for key, value in pairs(object:dynamicMetadata()) do
        object:dynamicMetadata():set("envoy.lb", "hello", "envoy")
      end
    end
  )EOF"};

  InSequence s;
  setup(SCRIPT);

  StreamInfo::StreamInfoImpl stream_info(Http::Protocol::Http2, test_time_.timeSystem());
  Filters::Common::Lua::LuaDeathRef<StreamInfoWrapper> wrapper(
      StreamInfoWrapper::create(coroutine_->luaState(), stream_info), true);
  EXPECT_THROW_WITH_MESSAGE(
      start("callMe"), Filters::Common::Lua::LuaException,
      "[string \"...\"]:5: dynamic metadata map cannot be modified while iterating");
}