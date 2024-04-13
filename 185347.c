HeaderUtility::getAllOfHeaderAsString(const HeaderMap& headers, const Http::LowerCaseString& key) {
  GetAllOfHeaderAsStringResult result;
  const auto header_value = headers.getAll(key);

  if (header_value.empty()) {
    // Empty for clarity. Avoid handling the empty case in the block below if the runtime feature
    // is disabled.
  } else if (header_value.size() == 1 ||
             !Runtime::runtimeFeatureEnabled(
                 "envoy.reloadable_features.http_match_on_all_headers")) {
    result.result_ = header_value[0]->value().getStringView();
  } else {
    // In this case we concatenate all found headers using a ',' delimiter before performing the
    // final match. We use an InlinedVector of absl::string_view to invoke the optimized join
    // algorithm. This requires a copying phase before we invoke join. The 3 used as the inline
    // size has been arbitrarily chosen.
    // TODO(mattklein123): Do we need to normalize any whitespace here?
    absl::InlinedVector<absl::string_view, 3> string_view_vector;
    string_view_vector.reserve(header_value.size());
    for (size_t i = 0; i < header_value.size(); i++) {
      string_view_vector.push_back(header_value[i]->value().getStringView());
    }
    result.result_backing_string_ = absl::StrJoin(string_view_vector, ",");
  }

  return result;
}