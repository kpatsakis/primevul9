FLAC_API FLAC__bool FLAC__stream_encoder_disable_verbatim_subframes(FLAC__StreamEncoder *encoder, FLAC__bool value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
	encoder->private_->disable_verbatim_subframes = value;
	return true;
}