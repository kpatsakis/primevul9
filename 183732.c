FLAC_API FLAC__bool FLAC__stream_encoder_set_total_samples_estimate(FLAC__StreamEncoder *encoder, FLAC__uint64 value)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(encoder->protected_->state != FLAC__STREAM_ENCODER_UNINITIALIZED)
		return false;
	value = flac_min(value, (FLAC__U64L(1) << FLAC__STREAM_METADATA_STREAMINFO_TOTAL_SAMPLES_LEN) - 1);
	encoder->protected_->total_samples_estimate = value;
	return true;
}