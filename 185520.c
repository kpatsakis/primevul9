  void setup(const std::string& script) override {
    Filters::Common::Lua::LuaWrappersTestBase<StreamInfoWrapper>::setup(script);
    state_->registerType<DynamicMetadataMapWrapper>();
    state_->registerType<DynamicMetadataMapIterator>();
  }