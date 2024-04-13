  JwtLocationBase(const std::string& token, const absl::node_hash_set<std::string>& issuers)
      : token_(token), specified_issuers_(issuers) {}