uint32_t evaluate_constant_subframe_(
	FLAC__StreamEncoder *encoder,
	const FLAC__int32 signal,
	uint32_t blocksize,
	uint32_t subframe_bps,
	FLAC__Subframe *subframe
)
{
	uint32_t estimate;
	subframe->type = FLAC__SUBFRAME_TYPE_CONSTANT;
	subframe->data.constant.value = signal;

	estimate = FLAC__SUBFRAME_ZERO_PAD_LEN + FLAC__SUBFRAME_TYPE_LEN + FLAC__SUBFRAME_WASTED_BITS_FLAG_LEN + subframe->wasted_bits + subframe_bps;

#if SPOTCHECK_ESTIMATE
	spotcheck_subframe_estimate_(encoder, blocksize, subframe_bps, subframe, estimate);
#else
	(void)encoder, (void)blocksize;
#endif

	return estimate;
}