bool HeaderUtility::matchHeaders(const HeaderMap& request_headers, const HeaderData& header_data) {
  const auto header_value = getAllOfHeaderAsString(request_headers, header_data.name_);

  if (!header_value.result().has_value()) {
    return header_data.invert_match_ && header_data.header_match_type_ == HeaderMatchType::Present;
  }

  bool match;
  switch (header_data.header_match_type_) {
  case HeaderMatchType::Value:
    match = header_data.value_.empty() || header_value.result().value() == header_data.value_;
    break;
  case HeaderMatchType::Regex:
    match = header_data.regex_->match(header_value.result().value());
    break;
  case HeaderMatchType::Range: {
    int64_t header_int_value = 0;
    match = absl::SimpleAtoi(header_value.result().value(), &header_int_value) &&
            header_int_value >= header_data.range_.start() &&
            header_int_value < header_data.range_.end();
    break;
  }
  case HeaderMatchType::Present:
    match = true;
    break;
  case HeaderMatchType::Prefix:
    match = absl::StartsWith(header_value.result().value(), header_data.value_);
    break;
  case HeaderMatchType::Suffix:
    match = absl::EndsWith(header_value.result().value(), header_data.value_);
    break;
  case HeaderMatchType::Contains:
    match = absl::StrContains(header_value.result().value(), header_data.value_);
    break;
  default:
    NOT_REACHED_GCOVR_EXCL_LINE;
  }

  return match != header_data.invert_match_;
}