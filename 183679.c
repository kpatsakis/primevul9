static inline uint32_t count_rice_bits_in_partition_(
	const uint32_t rice_parameter,
	const uint32_t partition_samples,
	const FLAC__int32 *residual
)
{
	uint32_t i;
	uint64_t partition_bits =
		FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_PARAMETER_LEN + /* actually could end up being FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_PARAMETER_LEN but err on side of 16bps */
		(1+rice_parameter) * partition_samples /* 1 for unary stop bit + rice_parameter for the binary portion */
	;
	for(i = 0; i < partition_samples; i++)
		partition_bits += ( (FLAC__uint32)((residual[i]<<1)^(residual[i]>>31)) >> rice_parameter );
	return (uint32_t)(flac_min(partition_bits,(uint32_t)(-1))); // To make sure the return value doesn't overflow
}