FLAC_API const char *FLAC__stream_encoder_get_resolved_state_string(const FLAC__StreamEncoder *encoder)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_VERIFY_DECODER_ERROR)
		return FLAC__StreamEncoderStateString[encoder->protected_->state];
	else
		return FLAC__stream_decoder_get_resolved_state_string(encoder->private_->verify.decoder);
}