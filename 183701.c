FLAC_API FLAC__bool FLAC__stream_encoder_set_do_escape_coding(FLAC__StreamEncoder *encoder, FLAC__bool value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
#if 0
	/*@@@ deprecated: */
	encoder->protected_->do_escape_coding = value;
#else
	(void)value;
#endif
	return true;
}