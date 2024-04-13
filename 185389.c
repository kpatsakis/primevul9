absl::optional<CelValue> ConnectionWrapper::operator[](CelValue key) const {
  if (!key.IsString()) {
    return {};
  }
  auto value = key.StringOrDie().value();
  if (value == MTLS) {
    return CelValue::CreateBool(info_.downstreamSslConnection() != nullptr &&
                                info_.downstreamSslConnection()->peerCertificatePresented());
  } else if (value == RequestedServerName) {
    return CelValue::CreateString(&info_.requestedServerName());
  }

  auto ssl_info = info_.downstreamSslConnection();
  if (ssl_info != nullptr) {
    return extractSslInfo(*ssl_info, value);
  }

  return {};
}