FLAC_API FLAC__bool FLAC__stream_encoder_set_rice_parameter_search_dist(FLAC__StreamEncoder *encoder, uint32_t value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
#if 0
	/*@@@ deprecated: */
	encoder->protected_->rice_parameter_search_dist = value;
#else
	(void)value;
#endif
	return true;
}