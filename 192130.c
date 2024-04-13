static inline u32 smca_ctl_reg(int bank)
{
	return MSR_AMD64_SMCA_MCx_CTL(bank);
}