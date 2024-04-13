void HeaderUtility::getAllOfHeader(const HeaderMap& headers, absl::string_view key,
                                   std::vector<absl::string_view>& out) {
  headers.iterate([key = LowerCaseString(std::string(key)),
                   &out](const HeaderEntry& header) -> HeaderMap::Iterate {
    if (header.key() == key.get().c_str()) {
      out.emplace_back(header.value().getStringView());
    }
    return HeaderMap::Iterate::Continue;
  });
}