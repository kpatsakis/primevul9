TEST_F(LuaStreamInfoWrapperTest, SetGetAndIterateDynamicMetadata) {
  const std::string SCRIPT{R"EOF(
      function callMe(object)
        testPrint(type(object:dynamicMetadata()))
        object:dynamicMetadata():set("envoy.lb", "foo", "bar")
        object:dynamicMetadata():set("envoy.lb", "so", "cool")

        testPrint(object:dynamicMetadata():get("envoy.lb")["foo"])
        testPrint(object:dynamicMetadata():get("envoy.lb")["so"])

        for filter, entry in pairs(object:dynamicMetadata()) do
          for key, value in pairs(entry) do
            testPrint(string.format("'%s' '%s'", key, value))
          end
        end

        local function nRetVals(...)
          return select('#',...)
        end
        testPrint(tostring(nRetVals(object:dynamicMetadata():get("envoy.ngx"))))
      end
    )EOF"};

  InSequence s;
  setup(SCRIPT);

  StreamInfo::StreamInfoImpl stream_info(Http::Protocol::Http2, test_time_.timeSystem());
  EXPECT_EQ(0, stream_info.dynamicMetadata().filter_metadata_size());
  Filters::Common::Lua::LuaDeathRef<StreamInfoWrapper> wrapper(
      StreamInfoWrapper::create(coroutine_->luaState(), stream_info), true);
  EXPECT_CALL(printer_, testPrint("userdata"));
  EXPECT_CALL(printer_, testPrint("bar"));
  EXPECT_CALL(printer_, testPrint("cool"));
  EXPECT_CALL(printer_, testPrint("'foo' 'bar'"));
  EXPECT_CALL(printer_, testPrint("'so' 'cool'"));
  EXPECT_CALL(printer_, testPrint("0"));
  start("callMe");

  EXPECT_EQ(1, stream_info.dynamicMetadata().filter_metadata_size());
  EXPECT_EQ("bar", stream_info.dynamicMetadata()
                       .filter_metadata()
                       .at("envoy.lb")
                       .fields()
                       .at("foo")
                       .string_value());
  wrapper.reset();
}