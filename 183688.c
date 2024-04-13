uint32_t evaluate_lpc_subframe_(
	FLAC__StreamEncoder *encoder,
	const FLAC__int32 signal[],
	FLAC__int32 residual[],
	FLAC__uint64 abs_residual_partition_sums[],
	uint32_t raw_bits_per_partition[],
	const FLAC__real lp_coeff[],
	uint32_t blocksize,
	uint32_t subframe_bps,
	uint32_t order,
	uint32_t qlp_coeff_precision,
	uint32_t rice_parameter,
	uint32_t rice_parameter_limit,
	uint32_t min_partition_order,
	uint32_t max_partition_order,
	FLAC__bool do_escape_coding,
	uint32_t rice_parameter_search_dist,
	FLAC__Subframe *subframe,
	FLAC__EntropyCodingMethod_PartitionedRiceContents *partitioned_rice_contents
)
{
	FLAC__int32 qlp_coeff[FLAC__MAX_LPC_ORDER]; /* WATCHOUT: the size is important; some x86 intrinsic routines need more than lpc order elements */
	uint32_t i, residual_bits, estimate;
	int quantization, ret;
	const uint32_t residual_samples = blocksize - order;

	/* try to keep qlp coeff precision such that only 32-bit math is required for decode of <=16bps(+1bps for side channel) streams */
	if(subframe_bps <= 17) {
		FLAC__ASSERT(order > 0);
		FLAC__ASSERT(order <= FLAC__MAX_LPC_ORDER);
		qlp_coeff_precision = flac_min(qlp_coeff_precision, 32 - subframe_bps - FLAC__bitmath_ilog2(order));
	}

	ret = FLAC__lpc_quantize_coefficients(lp_coeff, order, qlp_coeff_precision, qlp_coeff, &quantization);
	if(ret != 0)
		return 0; /* this is a hack to indicate to the caller that we can't do lp at this order on this subframe */

	if(subframe_bps + qlp_coeff_precision + FLAC__bitmath_ilog2(order) <= 32)
		if(subframe_bps <= 16 && qlp_coeff_precision <= 16)
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_16bit(signal+order, residual_samples, qlp_coeff, order, quantization, residual);
		else
			encoder->private_->local_lpc_compute_residual_from_qlp_coefficients(signal+order, residual_samples, qlp_coeff, order, quantization, residual);
	else
		encoder->private_->local_lpc_compute_residual_from_qlp_coefficients_64bit(signal+order, residual_samples, qlp_coeff, order, quantization, residual);

	subframe->type = FLAC__SUBFRAME_TYPE_LPC;

	subframe->data.lpc.entropy_coding_method.type = FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE;
	subframe->data.lpc.entropy_coding_method.data.partitioned_rice.contents = partitioned_rice_contents;
	subframe->data.lpc.residual = residual;

	residual_bits =
		find_best_partition_order_(
			encoder->private_,
			residual,
			abs_residual_partition_sums,
			raw_bits_per_partition,
			residual_samples,
			order,
			rice_parameter,
			rice_parameter_limit,
			min_partition_order,
			max_partition_order,
			subframe_bps,
			do_escape_coding,
			rice_parameter_search_dist,
			&subframe->data.lpc.entropy_coding_method
		);

	subframe->data.lpc.order = order;
	subframe->data.lpc.qlp_coeff_precision = qlp_coeff_precision;
	subframe->data.lpc.quantization_level = quantization;
	memcpy(subframe->data.lpc.qlp_coeff, qlp_coeff, sizeof(FLAC__int32)*FLAC__MAX_LPC_ORDER);
	for(i = 0; i < order; i++)
		subframe->data.lpc.warmup[i] = signal[i];

	estimate = FLAC__SUBFRAME_ZERO_PAD_LEN + FLAC__SUBFRAME_TYPE_LEN + FLAC__SUBFRAME_WASTED_BITS_FLAG_LEN + subframe->wasted_bits + FLAC__SUBFRAME_LPC_QLP_COEFF_PRECISION_LEN + FLAC__SUBFRAME_LPC_QLP_SHIFT_LEN + (order * (qlp_coeff_precision + subframe_bps)) + residual_bits;

#if SPOTCHECK_ESTIMATE
	spotcheck_subframe_estimate_(encoder, blocksize, subframe_bps, subframe, estimate);
#endif

	return estimate;
}