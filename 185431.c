ExtractorConstPtr Extractor::create(const JwtProviderList& providers) {
  return std::make_unique<ExtractorImpl>(providers);
}