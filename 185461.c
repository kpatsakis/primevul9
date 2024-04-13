bool Config::configToVector(const ProtobufRepeatedRule& proto_rules,
                            HeaderToMetadataRules& vector) {
  if (proto_rules.empty()) {
    ENVOY_LOG(debug, "no rules provided");
    return false;
  }

  for (const auto& entry : proto_rules) {
    vector.emplace_back(entry);
  }

  return true;
}