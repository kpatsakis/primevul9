void HeaderToMetadataFilter::applyKeyValue(std::string&& value, const Rule& rule,
                                           const KeyValuePair& keyval, StructMap& np) {
  if (!keyval.value().empty()) {
    value = keyval.value();
  } else {
    const auto& matcher = rule.regexRewrite();
    if (matcher != nullptr) {
      value = matcher->replaceAll(value, rule.regexSubstitution());
    }
  }
  if (!value.empty()) {
    const auto& nspace = decideNamespace(keyval.metadata_namespace());
    addMetadata(np, nspace, keyval.key(), value, keyval.type(), keyval.encode());
  } else {
    ENVOY_LOG(debug, "value is empty, not adding metadata");
  }
}