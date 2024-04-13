ExtractorImpl::ExtractorImpl(const JwtProviderList& providers) {
  for (const auto& provider : providers) {
    ASSERT(provider);
    addProvider(*provider);
  }
}