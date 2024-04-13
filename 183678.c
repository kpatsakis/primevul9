FLAC_API FLAC__bool FLAC__stream_encoder_set_do_md5(FLAC__StreamEncoder *encoder, FLAC__bool value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
	encoder->protected_->do_md5 = value;
	return true;
}