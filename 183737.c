FLAC_API FLAC__uint64 FLAC__stream_encoder_get_total_samples_estimate(const FLAC__StreamEncoder *encoder)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	return encoder->protected_->total_samples_estimate;
}