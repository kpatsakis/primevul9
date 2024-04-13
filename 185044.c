TEST_F(LuaHeaderMapWrapperTest, ModifyAfterIteration) {
  const std::string SCRIPT{R"EOF(
    function callMe(object)
      for key, value in pairs(object) do
        testPrint(string.format("'%s' '%s'", key, value))
      end

      object:add("hello", "world")

      for key, value in pairs(object) do
        testPrint(string.format("'%s' '%s'", key, value))
      end
    end
  )EOF"};

  InSequence s;
  setup(SCRIPT);

  Http::TestRequestHeaderMapImpl headers{{"foo", "bar"}};
  HeaderMapWrapper::create(coroutine_->luaState(), headers, []() { return true; });
  EXPECT_CALL(printer_, testPrint("'foo' 'bar'"));
  EXPECT_CALL(printer_, testPrint("'foo' 'bar'"));
  EXPECT_CALL(printer_, testPrint("'hello' 'world'"));
  start("callMe");
}