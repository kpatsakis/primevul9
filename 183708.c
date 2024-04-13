static inline uint32_t count_rice_bits_in_partition_(
	const uint32_t rice_parameter,
	const uint32_t partition_samples,
	const FLAC__uint64 abs_residual_partition_sum
)
{
	return (uint32_t)(flac_min( // To make sure the return value doesn't overflow
		FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE_PARAMETER_LEN + /* actually could end up being FLAC__ENTROPY_CODING_METHOD_PARTITIONED_RICE2_PARAMETER_LEN but err on side of 16bps */
		(1+rice_parameter) * partition_samples + /* 1 for unary stop bit + rice_parameter for the binary portion */
		(
			rice_parameter?
				(abs_residual_partition_sum >> (rice_parameter-1)) /* rice_parameter-1 because the real coder sign-folds instead of using a sign bit */
				: (abs_residual_partition_sum << 1) /* can't shift by negative number, so reverse */
		)
		- (partition_samples >> 1),(uint32_t)(-1)));
		/* -(partition_samples>>1) to subtract out extra contributions to the abs_residual_partition_sum.
		 * The actual number of bits used is closer to the sum(for all i in the partition) of  abs(residual[i])>>(rice_parameter-1)
		 * By using the abs_residual_partition sum, we also add in bits in the LSBs that would normally be shifted out.
		 * So the subtraction term tries to guess how many extra bits were contributed.
		 * If the LSBs are randomly distributed, this should average to 0.5 extra bits per sample.
		 */
	;
}