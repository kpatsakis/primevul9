void HeaderToMetadataFilter::setEncoderFilterCallbacks(
    Http::StreamEncoderFilterCallbacks& callbacks) {
  encoder_callbacks_ = &callbacks;
}