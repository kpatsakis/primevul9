static inline u32 smca_misc_reg(int bank)
{
	return MSR_AMD64_SMCA_MCx_MISC(bank);
}