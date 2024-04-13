absl::optional<CelValue> extractSslInfo(const Ssl::ConnectionInfo& ssl_info,
                                        absl::string_view value) {
  if (value == TLSVersion) {
    return CelValue::CreateString(&ssl_info.tlsVersion());
  } else if (value == SubjectLocalCertificate) {
    return CelValue::CreateString(&ssl_info.subjectLocalCertificate());
  } else if (value == SubjectPeerCertificate) {
    return CelValue::CreateString(&ssl_info.subjectPeerCertificate());
  } else if (value == URISanLocalCertificate) {
    if (!ssl_info.uriSanLocalCertificate().empty()) {
      return CelValue::CreateString(&ssl_info.uriSanLocalCertificate()[0]);
    }
  } else if (value == URISanPeerCertificate) {
    if (!ssl_info.uriSanPeerCertificate().empty()) {
      return CelValue::CreateString(&ssl_info.uriSanPeerCertificate()[0]);
    }
  } else if (value == DNSSanLocalCertificate) {
    if (!ssl_info.dnsSansLocalCertificate().empty()) {
      return CelValue::CreateString(&ssl_info.dnsSansLocalCertificate()[0]);
    }
  } else if (value == DNSSanPeerCertificate) {
    if (!ssl_info.dnsSansPeerCertificate().empty()) {
      return CelValue::CreateString(&ssl_info.dnsSansPeerCertificate()[0]);
    }
  }
  return {};
}