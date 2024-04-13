TEST_F(LuaStreamInfoWrapperTest, ModifyAfterIterationForDynamicMetadata) {
  const std::string SCRIPT{R"EOF(
    function callMe(object)
      object:dynamicMetadata():set("envoy.lb", "hello", "world")
      for filter, entry in pairs(object:dynamicMetadata()) do
        testPrint(filter)
        for key, value in pairs(entry) do
          testPrint(string.format("'%s' '%s'", key, value))
        end
      end

      object:dynamicMetadata():set("envoy.lb", "hello", "envoy")
      object:dynamicMetadata():set("envoy.proxy", "proto", "grpc")
      for filter, entry in pairs(object:dynamicMetadata()) do
        testPrint(filter)
        for key, value in pairs(entry) do
          testPrint(string.format("'%s' '%s'", key, value))
        end
      end
    end
  )EOF"};

  InSequence s;
  setup(SCRIPT);

  StreamInfo::StreamInfoImpl stream_info(Http::Protocol::Http2, test_time_.timeSystem());
  EXPECT_EQ(0, stream_info.dynamicMetadata().filter_metadata_size());
  Filters::Common::Lua::LuaDeathRef<StreamInfoWrapper> wrapper(
      StreamInfoWrapper::create(coroutine_->luaState(), stream_info), true);
  EXPECT_CALL(printer_, testPrint("envoy.lb"));
  EXPECT_CALL(printer_, testPrint("'hello' 'world'"));
  EXPECT_CALL(printer_, testPrint("envoy.proxy"));
  EXPECT_CALL(printer_, testPrint("'proto' 'grpc'"));
  EXPECT_CALL(printer_, testPrint("envoy.lb"));
  EXPECT_CALL(printer_, testPrint("'hello' 'envoy'"));
  start("callMe");
}