absl::optional<CelValue> PeerWrapper::operator[](CelValue key) const {
  if (!key.IsString()) {
    return {};
  }
  auto value = key.StringOrDie().value();
  if (value == Address) {
    if (local_) {
      return CelValue::CreateStringView(info_.downstreamLocalAddress()->asStringView());
    } else {
      return CelValue::CreateStringView(info_.downstreamRemoteAddress()->asStringView());
    }
  } else if (value == Port) {
    if (local_) {
      if (info_.downstreamLocalAddress()->ip() != nullptr) {
        return CelValue::CreateInt64(info_.downstreamLocalAddress()->ip()->port());
      }
    } else {
      if (info_.downstreamRemoteAddress()->ip() != nullptr) {
        return CelValue::CreateInt64(info_.downstreamRemoteAddress()->ip()->port());
      }
    }
  }

  return {};
}