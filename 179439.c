static inline u64 gen8_noncanonical_addr(u64 address)
{
	return address & GENMASK_ULL(GEN8_HIGH_ADDRESS_BIT, 0);
}