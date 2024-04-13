TEST_F(LuaHeaderMapWrapperTest, ModifyDuringIteration) {
  const std::string SCRIPT{R"EOF(
    function callMe(object)
      for key, value in pairs(object) do
        object:add("hello", "world")
      end
    end
  )EOF"};

  InSequence s;
  setup(SCRIPT);

  Http::TestRequestHeaderMapImpl headers{{"foo", "bar"}};
  HeaderMapWrapper::create(coroutine_->luaState(), headers, []() { return true; });
  EXPECT_THROW_WITH_MESSAGE(start("callMe"), Filters::Common::Lua::LuaException,
                            "[string \"...\"]:4: header map cannot be modified while iterating");
}