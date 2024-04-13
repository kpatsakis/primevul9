ExtractorConstPtr Extractor::create(const JwtProvider& provider) {
  return std::make_unique<ExtractorImpl>(provider);
}