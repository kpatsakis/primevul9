TEST_F(LuaStreamInfoWrapperTest, BadTypesInTableForDynamicMetadata) {
  const std::string SCRIPT{R"EOF(
    function callMe(object)
      object:dynamicMetadata():set("envoy.lb", "hello", {x="world", y=function(a, b) end})
    end
  )EOF"};

  InSequence s;
  setup(SCRIPT);

  StreamInfo::StreamInfoImpl stream_info(Http::Protocol::Http2, test_time_.timeSystem());
  Filters::Common::Lua::LuaDeathRef<StreamInfoWrapper> wrapper(
      StreamInfoWrapper::create(coroutine_->luaState(), stream_info), true);
  EXPECT_THROW_WITH_MESSAGE(start("callMe"), Filters::Common::Lua::LuaException,
                            "[string \"...\"]:3: unexpected type 'function' in dynamicMetadata");
}