  bool isIssuerSpecified(const std::string& issuer) const override {
    return specified_issuers_.find(issuer) != specified_issuers_.end();
  }