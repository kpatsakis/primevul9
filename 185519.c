  void initializeFilter(const std::string& yaml) {
    envoy::extensions::filters::http::header_to_metadata::v3::Config config;
    TestUtility::loadFromYaml(yaml, config);
    config_ = std::make_shared<Config>(config);
    filter_ = std::make_shared<HeaderToMetadataFilter>(config_);
    filter_->setDecoderFilterCallbacks(decoder_callbacks_);
    filter_->setEncoderFilterCallbacks(encoder_callbacks_);
  }