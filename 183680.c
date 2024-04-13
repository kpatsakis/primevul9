FLAC_API FLAC__bool FLAC__stream_encoder_set_qlp_coeff_precision(FLAC__StreamEncoder *encoder, uint32_t value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
	encoder->protected_->qlp_coeff_precision = value;
	return true;
}