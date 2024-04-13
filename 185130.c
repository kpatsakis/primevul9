  void setup(const std::string& script) override {
    Filters::Common::Lua::LuaWrappersTestBase<HeaderMapWrapper>::setup(script);
    state_->registerType<HeaderMapIterator>();
  }