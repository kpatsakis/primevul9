uint32_t evaluate_fixed_subframe_(
	FLAC__StreamEncoder *encoder,
	const FLAC__int32 signal[],
	FLAC__int32 residual[],
	FLAC__uint64 abs_residual_partition_sums[],
	uint32_t raw_bits_per_partition[],
	uint32_t blocksize,
	uint32_t subframe_bps,
	uint32_t order,
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
	uint32_t i, residual_bits, estimate;
	const uint32_t residual_samples = blocksize - order;

	FLAC__fixed_compute_residual(signal+order, residual_samples, order, residual);

	subframe->type = FLAC__SUBFRAME_TYPE_FIXED;

	subframe->data.fixed.entropy_coding_method.type = FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE;
	subframe->data.fixed.entropy_coding_method.data.partitioned_rice.contents = partitioned_rice_contents;
	subframe->data.fixed.residual = residual;

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
			&subframe->data.fixed.entropy_coding_method
		);

	subframe->data.fixed.order = order;
	for(i = 0; i < order; i++)
		subframe->data.fixed.warmup[i] = signal[i];

	estimate = FLAC__SUBFRAME_ZERO_PAD_LEN + FLAC__SUBFRAME_TYPE_LEN + FLAC__SUBFRAME_WASTED_BITS_FLAG_LEN + subframe->wasted_bits + (order * subframe_bps) + residual_bits;

#if SPOTCHECK_ESTIMATE
	spotcheck_subframe_estimate_(encoder, blocksize, subframe_bps, subframe, estimate);
#endif

	return estimate;
}