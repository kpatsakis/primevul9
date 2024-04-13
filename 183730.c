FLAC_API FLAC__bool FLAC__stream_encoder_get_do_qlp_coeff_prec_search(const FLAC__StreamEncoder *encoder)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	return encoder->protected_->do_qlp_coeff_prec_search;
}