static inline u32 smca_addr_reg(int bank)
{
	return MSR_AMD64_SMCA_MCx_ADDR(bank);
}