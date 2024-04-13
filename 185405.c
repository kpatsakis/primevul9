bool HeaderUtility::headerValueIsValid(const absl::string_view header_value) {
  return nghttp2_check_header_value(reinterpret_cast<const uint8_t*>(header_value.data()),
                                    header_value.size()) != 0;
}