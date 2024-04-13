static inline u32 status_reg(int bank)
{
	return MSR_IA32_MCx_STATUS(bank);
}