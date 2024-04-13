FLAC_API FLAC__bool FLAC__stream_encoder_set_ogg_serial_number(FLAC__StreamEncoder *encoder, long value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
#if FLAC__HAS_OGG
	/* can't check encoder->private_->is_ogg since that's not set until init time */
	FLAC__ogg_encoder_aspect_set_serial_number(&encoder->protected_->ogg_encoder_aspect, value);
	return true;
#else
	(void)value;
	return false;
#endif
}