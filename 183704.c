FLAC_API FLAC__bool FLAC__stream_encoder_set_loose_mid_side_stereo(FLAC__StreamEncoder *encoder, FLAC__bool value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
	encoder->protected_->loose_mid_side_stereo = value;
	return true;
}