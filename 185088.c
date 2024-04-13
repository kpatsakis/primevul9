Http::FilterHeadersStatus HeaderToMetadataFilter::decodeHeaders(Http::RequestHeaderMap& headers,
                                                                bool) {
  const auto* config = getConfig();
  if (config->doRequest()) {
    writeHeaderToMetadata(headers, config->requestRules(), *decoder_callbacks_);
  }

  return Http::FilterHeadersStatus::Continue;
}