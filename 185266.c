TEST_F(LuaStreamInfoWrapperTest, SetGetComplexDynamicMetadata) {
  const std::string SCRIPT{R"EOF(
      function callMe(object)
        object:dynamicMetadata():set("envoy.lb", "foo", {x=1234, y="baz", z=true})
        object:dynamicMetadata():set("envoy.lb", "so", {"cool", "and", "dynamic", true})

        testPrint(tostring(object:dynamicMetadata():get("envoy.lb")["foo"].x))
        testPrint(object:dynamicMetadata():get("envoy.lb")["foo"].y)
        testPrint(tostring(object:dynamicMetadata():get("envoy.lb")["foo"].z))
        testPrint(object:dynamicMetadata():get("envoy.lb")["so"][1])
        testPrint(object:dynamicMetadata():get("envoy.lb")["so"][2])
        testPrint(object:dynamicMetadata():get("envoy.lb")["so"][3])
        testPrint(tostring(object:dynamicMetadata():get("envoy.lb")["so"][4]))
      end
    )EOF"};

  InSequence s;
  setup(SCRIPT);

  StreamInfo::StreamInfoImpl stream_info(Http::Protocol::Http2, test_time_.timeSystem());
  EXPECT_EQ(0, stream_info.dynamicMetadata().filter_metadata_size());
  Filters::Common::Lua::LuaDeathRef<StreamInfoWrapper> wrapper(
      StreamInfoWrapper::create(coroutine_->luaState(), stream_info), true);
  EXPECT_CALL(printer_, testPrint("1234"));
  EXPECT_CALL(printer_, testPrint("baz"));
  EXPECT_CALL(printer_, testPrint("true"));
  EXPECT_CALL(printer_, testPrint("cool"));
  EXPECT_CALL(printer_, testPrint("and"));
  EXPECT_CALL(printer_, testPrint("dynamic"));
  EXPECT_CALL(printer_, testPrint("true"));
  start("callMe");

  EXPECT_EQ(1, stream_info.dynamicMetadata().filter_metadata_size());
  const ProtobufWkt::Struct& meta_foo = stream_info.dynamicMetadata()
                                            .filter_metadata()
                                            .at("envoy.lb")
                                            .fields()
                                            .at("foo")
                                            .struct_value();

  EXPECT_EQ(1234.0, meta_foo.fields().at("x").number_value());
  EXPECT_EQ("baz", meta_foo.fields().at("y").string_value());
  EXPECT_EQ(true, meta_foo.fields().at("z").bool_value());

  const ProtobufWkt::ListValue& meta_so =
      stream_info.dynamicMetadata().filter_metadata().at("envoy.lb").fields().at("so").list_value();

  EXPECT_EQ(4, meta_so.values_size());
  EXPECT_EQ("cool", meta_so.values(0).string_value());
  EXPECT_EQ("and", meta_so.values(1).string_value());
  EXPECT_EQ("dynamic", meta_so.values(2).string_value());
  EXPECT_EQ(true, meta_so.values(3).bool_value());

  wrapper.reset();
}