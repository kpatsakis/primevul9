bool HeaderUtility::authorityIsValid(const absl::string_view header_value) {
  return nghttp2_check_authority(reinterpret_cast<const uint8_t*>(header_value.data()),
                                 header_value.size()) != 0;
}