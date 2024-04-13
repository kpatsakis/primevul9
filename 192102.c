static inline u32 ctl_reg(int bank)
{
	return MSR_IA32_MCx_CTL(bank);
}