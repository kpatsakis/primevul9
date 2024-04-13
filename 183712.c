FLAC_API void FLAC__stream_encoder_get_verify_decoder_error_stats(const FLAC__StreamEncoder *encoder, FLAC__uint64 *absolute_sample, uint32_t *frame_number, uint32_t *channel, uint32_t *sample, FLAC__int32 *expected, FLAC__int32 *got)
{
	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	if(0 != absolute_sample)
		*absolute_sample = encoder->private_->verify.error_stats.absolute_sample;
	if(0 != frame_number)
		*frame_number = encoder->private_->verify.error_stats.frame_number;
	if(0 != channel)
		*channel = encoder->private_->verify.error_stats.channel;
	if(0 != sample)
		*sample = encoder->private_->verify.error_stats.sample;
	if(0 != expected)
		*expected = encoder->private_->verify.error_stats.expected;
	if(0 != got)
		*got = encoder->private_->verify.error_stats.got;
}