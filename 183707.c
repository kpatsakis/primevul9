FLAC_API FLAC__bool FLAC__stream_encoder_set_do_qlp_coeff_prec_search(FLAC__StreamEncoder *encoder, FLAC__bool value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
	encoder->protected_->do_qlp_coeff_prec_search = value;
	return true;
}