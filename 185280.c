bool HeaderToMetadataFilter::addMetadata(StructMap& map, const std::string& meta_namespace,
                                         const std::string& key, std::string value, ValueType type,
                                         ValueEncode encode) const {
  ProtobufWkt::Value val;

  ASSERT(!value.empty());

  if (value.size() >= MAX_HEADER_VALUE_LEN) {
    // Too long, go away.
    ENVOY_LOG(debug, "metadata value is too long");
    return false;
  }

  if (encode == envoy::extensions::filters::http::header_to_metadata::v3::Config::BASE64) {
    value = Base64::decodeWithoutPadding(value);
    if (value.empty()) {
      ENVOY_LOG(debug, "Base64 decode failed");
      return false;
    }
  }

  // Sane enough, add the key/value.
  switch (type) {
  case envoy::extensions::filters::http::header_to_metadata::v3::Config::STRING:
    val.set_string_value(std::move(value));
    break;
  case envoy::extensions::filters::http::header_to_metadata::v3::Config::NUMBER: {
    double dval;
    if (absl::SimpleAtod(StringUtil::trim(value), &dval)) {
      val.set_number_value(dval);
    } else {
      ENVOY_LOG(debug, "value to number conversion failed");
      return false;
    }
    break;
  }
  case envoy::extensions::filters::http::header_to_metadata::v3::Config::PROTOBUF_VALUE: {
    if (!val.ParseFromString(value)) {
      ENVOY_LOG(debug, "parse from decoded string failed");
      return false;
    }
    break;
  }
  default:
    NOT_REACHED_GCOVR_EXCL_LINE;
  }

  // Have we seen this namespace before?
  auto namespace_iter = map.find(meta_namespace);
  if (namespace_iter == map.end()) {
    map[meta_namespace] = ProtobufWkt::Struct();
    namespace_iter = map.find(meta_namespace);
  }

  auto& keyval = namespace_iter->second;
  (*keyval.mutable_fields())[key] = val;

  return true;
}