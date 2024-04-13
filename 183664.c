FLAC_API uint32_t FLAC__stream_encoder_get_rice_parameter_search_dist(const FLAC__StreamEncoder *encoder)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	return encoder->protected_->rice_parameter_search_dist;
}