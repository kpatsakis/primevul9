  void SetUp() override {
    TestUtility::loadFromYaml(ExampleConfig, config_);
    JwtProviderList providers;
    for (const auto& it : config_.providers()) {
      providers.emplace_back(&it.second);
    }
    extractor_ = Extractor::create(providers);
  }