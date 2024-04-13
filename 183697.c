void precompute_partition_info_escapes_(
	const FLAC__int32 residual[],
	uint32_t raw_bits_per_partition[],
	uint32_t residual_samples,
	uint32_t predictor_order,
	uint32_t min_partition_order,
	uint32_t max_partition_order
)
{
	int partition_order;
	uint32_t from_partition, to_partition = 0;
	const uint32_t blocksize = residual_samples + predictor_order;

	/* first do max_partition_order */
	for(partition_order = (int)max_partition_order; partition_order >= 0; partition_order--) {
		FLAC__int32 r;
		FLAC__uint32 rmax;
		uint32_t partition, partition_sample, partition_samples, residual_sample;
		const uint32_t partitions = 1u << partition_order;
		const uint32_t default_partition_samples = blocksize >> partition_order;

		FLAC__ASSERT(default_partition_samples > predictor_order);

		for(partition = residual_sample = 0; partition < partitions; partition++) {
			partition_samples = default_partition_samples;
			if(partition == 0)
				partition_samples -= predictor_order;
			rmax = 0;
			for(partition_sample = 0; partition_sample < partition_samples; partition_sample++) {
				r = residual[residual_sample++];
				/* OPT: maybe faster: rmax |= r ^ (r>>31) */
				if(r < 0)
					rmax |= ~r;
				else
					rmax |= r;
			}
			/* now we know all residual values are in the range [-rmax-1,rmax] */
			raw_bits_per_partition[partition] = rmax? FLAC__bitmath_ilog2(rmax) + 2 : 1;
		}
		to_partition = partitions;
		break; /*@@@ yuck, should remove the 'for' loop instead */
	}

	/* now merge partitions for lower orders */
	for(from_partition = 0, --partition_order; partition_order >= (int)min_partition_order; partition_order--) {
		uint32_t m;
		uint32_t i;
		const uint32_t partitions = 1u << partition_order;
		for(i = 0; i < partitions; i++) {
			m = raw_bits_per_partition[from_partition];
			from_partition++;
			raw_bits_per_partition[to_partition] = flac_max(m, raw_bits_per_partition[from_partition]);
			from_partition++;
			to_partition++;
		}
	}
}