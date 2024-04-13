FLAC_API FLAC__StreamDecoderState FLAC__stream_encoder_get_verify_decoder_state(const FLAC__StreamEncoder *encoder)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->verify)
		return FLAC__stream_decoder_get_state(encoder->private_->verify.decoder);
	else
		return FLAC__STREAM_DECODER_UNINITIALIZED;
}