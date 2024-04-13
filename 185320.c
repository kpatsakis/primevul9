void HeaderMapImpl::setCopy(const LowerCaseString& key, absl::string_view value) {
  if (!Runtime::runtimeFeatureEnabled(
          "envoy.reloadable_features.http_set_copy_replace_all_headers")) {
    auto entry = getExisting(key);
    if (!entry.empty()) {
      updateSize(entry[0]->value().size(), value.size());
      entry[0]->value(value);
    } else {
      addCopy(key, value);
    }
  } else {
    remove(key);
    addCopy(key, value);
  }
}