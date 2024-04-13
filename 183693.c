FLAC_API FLAC__bool FLAC__stream_encoder_set_max_residual_partition_order(FLAC__StreamEncoder *encoder, uint32_t value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
	encoder->protected_->max_residual_partition_order = value;
	return true;
}