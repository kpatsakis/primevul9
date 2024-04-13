void ExtractorImpl::sanitizePayloadHeaders(Http::HeaderMap& headers) const {
  for (const auto& header : forward_payload_headers_) {
    headers.remove(header);
  }
}