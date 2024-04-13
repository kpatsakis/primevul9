  JwtHeaderLocation(const std::string& token, const absl::node_hash_set<std::string>& issuers,
                    const LowerCaseString& header)
      : JwtLocationBase(token, issuers), header_(header) {}