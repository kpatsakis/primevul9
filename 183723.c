uint32_t find_best_partition_order_(
	FLAC__StreamEncoderPrivate *private_,
	const FLAC__int32 residual[],
	FLAC__uint64 abs_residual_partition_sums[],
	uint32_t raw_bits_per_partition[],
	uint32_t residual_samples,
	uint32_t predictor_order,
	uint32_t rice_parameter,
	uint32_t rice_parameter_limit,
	uint32_t min_partition_order,
	uint32_t max_partition_order,
	uint32_t bps,
	FLAC__bool do_escape_coding,
	uint32_t rice_parameter_search_dist,
	FLAC__EntropyCodingMethod *best_ecm
)
{
	uint32_t residual_bits, best_residual_bits = 0;
	uint32_t best_parameters_index = 0;
	uint32_t best_partition_order = 0;
	const uint32_t blocksize = residual_samples + predictor_order;

	max_partition_order = FLAC__format_get_max_rice_partition_order_from_blocksize_limited_max_and_predictor_order(max_partition_order, blocksize, predictor_order);
	min_partition_order = flac_min(min_partition_order, max_partition_order);

	private_->local_precompute_partition_info_sums(residual, abs_residual_partition_sums, residual_samples, predictor_order, min_partition_order, max_partition_order, bps);

	if(do_escape_coding)
		precompute_partition_info_escapes_(residual, raw_bits_per_partition, residual_samples, predictor_order, min_partition_order, max_partition_order);

	{
		int partition_order;
		uint32_t sum;

		for(partition_order = (int)max_partition_order, sum = 0; partition_order >= (int)min_partition_order; partition_order--) {
			if(!
				set_partitioned_rice_(
#ifdef EXACT_RICE_BITS_CALCULATION
					residual,
#endif
					abs_residual_partition_sums+sum,
					raw_bits_per_partition+sum,
					residual_samples,
					predictor_order,
					rice_parameter,
					rice_parameter_limit,
					rice_parameter_search_dist,
					(uint32_t)partition_order,
					do_escape_coding,
					&private_->partitioned_rice_contents_extra[!best_parameters_index],
					&residual_bits
				)
			)
			{
				FLAC__ASSERT(best_residual_bits != 0);
				break;
			}
			sum += 1u << partition_order;
			if(best_residual_bits == 0 || residual_bits < best_residual_bits) {
				best_residual_bits = residual_bits;
				best_parameters_index = !best_parameters_index;
				best_partition_order = partition_order;
			}
		}
	}

	best_ecm->data.partitioned_rice.order = best_partition_order;

	{
		/*
		 * We are allowed to de-const the pointer based on our special
		 * knowledge; it is const to the outside world.
		 */
		FLAC__EntropyCodingMethod_PartitionedRiceContents* prc = (FLAC__EntropyCodingMethod_PartitionedRiceContents*)best_ecm->data.partitioned_rice.contents;
		uint32_t partition;

		/* save best parameters and raw_bits */
		FLAC__format_entropy_coding_method_partitioned_rice_contents_ensure_size(prc, flac_max(6u, best_partition_order));
		memcpy(prc->parameters, private_->partitioned_rice_contents_extra[best_parameters_index].parameters, sizeof(uint32_t)*(1<<(best_partition_order)));
		if(do_escape_coding)
			memcpy(prc->raw_bits, private_->partitioned_rice_contents_extra[best_parameters_index].raw_bits, sizeof(uint32_t)*(1<<(best_partition_order)));
		/*
		 * Now need to check if the type should be changed to
		 * FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2 based on the
		 * size of the rice parameters.
		 */
		for(partition = 0; partition < (1u<<best_partition_order); partition++) {
			if(prc->parameters[partition] >= FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_ESCAPE_PARAMETER) {
				best_ecm->type = FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2;
				break;
			}
		}
	}

	return best_residual_bits;
}