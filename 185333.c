Http::FilterHeadersStatus HeaderToMetadataFilter::encodeHeaders(Http::ResponseHeaderMap& headers,
                                                                bool) {
  const auto* config = getConfig();
  if (config->doResponse()) {
    writeHeaderToMetadata(headers, config->responseRules(), *encoder_callbacks_);
  }
  return Http::FilterHeadersStatus::Continue;
}