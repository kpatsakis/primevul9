  JwtParamLocation(const std::string& token, const absl::node_hash_set<std::string>& issuers,
                   const std::string&)
      : JwtLocationBase(token, issuers) {}