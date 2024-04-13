TEST_F(LuaStreamInfoWrapperTest, DontFinishIterationForDynamicMetadata) {
  const std::string SCRIPT{R"EOF(
    function callMe(object)
      object:dynamicMetadata():set("envoy.lb", "foo", "bar")
      iterator = pairs(object:dynamicMetadata())
      key, value = iterator()
      iterator2 = pairs(object:dynamicMetadata())
    end
  )EOF"};

  InSequence s;
  setup(SCRIPT);

  StreamInfo::StreamInfoImpl stream_info(Http::Protocol::Http2, test_time_.timeSystem());
  Filters::Common::Lua::LuaDeathRef<StreamInfoWrapper> wrapper(
      StreamInfoWrapper::create(coroutine_->luaState(), stream_info), true);
  EXPECT_THROW_WITH_MESSAGE(
      start("callMe"), Filters::Common::Lua::LuaException,
      "[string \"...\"]:6: cannot create a second iterator before completing the first");
}