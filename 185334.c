absl::optional<CelValue> UpstreamWrapper::operator[](CelValue key) const {
  if (!key.IsString()) {
    return {};
  }
  auto value = key.StringOrDie().value();
  if (value == Address) {
    auto upstream_host = info_.upstreamHost();
    if (upstream_host != nullptr && upstream_host->address() != nullptr) {
      return CelValue::CreateStringView(upstream_host->address()->asStringView());
    }
  } else if (value == Port) {
    auto upstream_host = info_.upstreamHost();
    if (upstream_host != nullptr && upstream_host->address() != nullptr &&
        upstream_host->address()->ip() != nullptr) {
      return CelValue::CreateInt64(upstream_host->address()->ip()->port());
    }
  } else if (value == UpstreamLocalAddress) {
    auto upstream_local_address = info_.upstreamLocalAddress();
    if (upstream_local_address != nullptr) {
      return CelValue::CreateStringView(upstream_local_address->asStringView());
    }
  } else if (value == UpstreamTransportFailureReason) {
    return CelValue::CreateStringView(info_.upstreamTransportFailureReason());
  }

  auto ssl_info = info_.upstreamSslConnection();
  if (ssl_info != nullptr) {
    return extractSslInfo(*ssl_info, value);
  }

  return {};
}