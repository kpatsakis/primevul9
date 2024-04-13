static inline u64 gen8_canonical_addr(u64 address)
{
	return sign_extend64(address, GEN8_HIGH_ADDRESS_BIT);
}