uint32_t evaluate_verbatim_subframe_(
	FLAC__StreamEncoder *encoder,
	const FLAC__int32 signal[],
	uint32_t blocksize,
	uint32_t subframe_bps,
	FLAC__Subframe *subframe
)
{
	uint32_t estimate;

	subframe->type = FLAC__SUBFRAME_TYPE_VERBATIM;

	subframe->data.verbatim.data = signal;

	estimate = FLAC__SUBFRAME_ZERO_PAD_LEN + FLAC__SUBFRAME_TYPE_LEN + FLAC__SUBFRAME_WASTED_BITS_FLAG_LEN + subframe->wasted_bits + (blocksize * subframe_bps);

#if SPOTCHECK_ESTIMATE
	spotcheck_subframe_estimate_(encoder, blocksize, subframe_bps, subframe, estimate);
#else
	(void)encoder;
#endif

	return estimate;
}