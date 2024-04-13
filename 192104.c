static inline u32 addr_reg(int bank)
{
	return MSR_IA32_MCx_ADDR(bank);
}