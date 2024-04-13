void precompute_partition_info_sums_(
	const FLAC__int32 residual[],
	FLAC__uint64 abs_residual_partition_sums[],
	uint32_t residual_samples,
	uint32_t predictor_order,
	uint32_t min_partition_order,
	uint32_t max_partition_order,
	uint32_t bps
)
{
	const uint32_t default_partition_samples = (residual_samples + predictor_order) >> max_partition_order;
	uint32_t partitions = 1u << max_partition_order;

	FLAC__ASSERT(default_partition_samples > predictor_order);

	/* first do max_partition_order */
	{
		const uint32_t threshold = 32 - FLAC__bitmath_ilog2(default_partition_samples);
		uint32_t partition, residual_sample, end = (uint32_t)(-(int)predictor_order);
		/* WATCHOUT: "bps + FLAC__MAX_EXTRA_RESIDUAL_BPS" is the maximum assumed size of the average residual magnitude */
		if(bps + FLAC__MAX_EXTRA_RESIDUAL_BPS < threshold) {
			for(partition = residual_sample = 0; partition < partitions; partition++) {
				FLAC__uint32 abs_residual_partition_sum = 0;
				end += default_partition_samples;
				for( ; residual_sample < end; residual_sample++)
					abs_residual_partition_sum += abs(residual[residual_sample]); /* abs(INT_MIN) is undefined, but if the residual is INT_MIN we have bigger problems */
				abs_residual_partition_sums[partition] = abs_residual_partition_sum;
			}
		}
		else { /* have to pessimistically use 64 bits for accumulator */
			for(partition = residual_sample = 0; partition < partitions; partition++) {
				FLAC__uint64 abs_residual_partition_sum64 = 0;
				end += default_partition_samples;
				for( ; residual_sample < end; residual_sample++)
					abs_residual_partition_sum64 += abs(residual[residual_sample]); /* abs(INT_MIN) is undefined, but if the residual is INT_MIN we have bigger problems */
				abs_residual_partition_sums[partition] = abs_residual_partition_sum64;
			}
		}
	}

	/* now merge partitions for lower orders */
	{
		uint32_t from_partition = 0, to_partition = partitions;
		int partition_order;
		for(partition_order = (int)max_partition_order - 1; partition_order >= (int)min_partition_order; partition_order--) {
			uint32_t i;
			partitions >>= 1;
			for(i = 0; i < partitions; i++) {
				abs_residual_partition_sums[to_partition++] =
					abs_residual_partition_sums[from_partition  ] +
					abs_residual_partition_sums[from_partition+1];
				from_partition += 2;
			}
		}
	}
}