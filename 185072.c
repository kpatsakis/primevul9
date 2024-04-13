void HeaderUtility::stripPortFromHost(RequestHeaderMap& headers, uint32_t listener_port) {

  if (headers.getMethodValue() == Http::Headers::get().MethodValues.Connect) {
    // According to RFC 2817 Connect method should have port part in host header.
    // In this case we won't strip it even if configured to do so.
    return;
  }
  const absl::string_view original_host = headers.getHostValue();
  const absl::string_view::size_type port_start = original_host.rfind(':');
  if (port_start == absl::string_view::npos) {
    return;
  }
  // According to RFC3986 v6 address is always enclosed in "[]". section 3.2.2.
  const auto v6_end_index = original_host.rfind("]");
  if (v6_end_index == absl::string_view::npos || v6_end_index < port_start) {
    if ((port_start + 1) > original_host.size()) {
      return;
    }
    const absl::string_view port_str = original_host.substr(port_start + 1);
    uint32_t port = 0;
    if (!absl::SimpleAtoi(port_str, &port)) {
      return;
    }
    if (port != listener_port) {
      // We would strip ports only if they are the same, as local port of the listener.
      return;
    }
    const absl::string_view host = original_host.substr(0, port_start);
    headers.setHost(host);
  }
}