void HeaderToMetadataFilter::writeHeaderToMetadata(Http::HeaderMap& headers,
                                                   const HeaderToMetadataRules& rules,
                                                   Http::StreamFilterCallbacks& callbacks) {
  StructMap structs_by_namespace;

  for (const auto& rule : rules) {
    const auto& proto_rule = rule.rule();
    absl::optional<std::string> value = rule.selector_->extract(headers);

    if (value && proto_rule.has_on_header_present()) {
      applyKeyValue(std::move(value).value_or(""), rule, proto_rule.on_header_present(),
                    structs_by_namespace);
    } else if (!value && proto_rule.has_on_header_missing()) {
      applyKeyValue(std::move(value).value_or(""), rule, proto_rule.on_header_missing(),
                    structs_by_namespace);
    }
  }
  // Any matching rules?
  if (!structs_by_namespace.empty()) {
    for (auto const& entry : structs_by_namespace) {
      callbacks.streamInfo().setDynamicMetadata(entry.first, entry.second);
    }
  }
}