const Config* HeaderToMetadataFilter::getConfig() const {
  // Cached config pointer.
  if (effective_config_) {
    return effective_config_;
  }

  effective_config_ = Http::Utility::resolveMostSpecificPerFilterConfig<Config>(
      HttpFilterNames::get().HeaderToMetadata, decoder_callbacks_->route());
  if (effective_config_) {
    return effective_config_;
  }

  effective_config_ = config_.get();
  return effective_config_;
}