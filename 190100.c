static inline u64 reserved_hpa_bits(void)
{
	return rsvd_bits(shadow_phys_bits, 63);
}