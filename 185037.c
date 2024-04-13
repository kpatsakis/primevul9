TEST_F(LuaHeaderMapWrapperTest, Methods) {
  const std::string SCRIPT{R"EOF(
    function callMe(object)
      object:add("HELLO", "WORLD")
      testPrint(object:get("hELLo"))

      object:add("header1", "")
      object:add("header2", "foo")

      for key, value in pairs(object) do
        testPrint(string.format("'%s' '%s'", key, value))
      end

      object:remove("header1")
      for key, value in pairs(object) do
        testPrint(string.format("'%s' '%s'", key, value))
      end

      object:add("header3", "foo")
      object:add("header3", "bar")
      testPrint(object:get("header3"))
    end
  )EOF"};

  InSequence s;
  setup(SCRIPT);

  Http::TestRequestHeaderMapImpl headers;
  HeaderMapWrapper::create(coroutine_->luaState(), headers, []() { return true; });
  EXPECT_CALL(printer_, testPrint("WORLD"));
  EXPECT_CALL(printer_, testPrint("'hello' 'WORLD'"));
  EXPECT_CALL(printer_, testPrint("'header1' ''"));
  EXPECT_CALL(printer_, testPrint("'header2' 'foo'"));
  EXPECT_CALL(printer_, testPrint("'hello' 'WORLD'"));
  EXPECT_CALL(printer_, testPrint("'header2' 'foo'"));
  EXPECT_CALL(printer_, testPrint("foo,bar"));
  start("callMe");
}