static inline void copy_kernel_to_xregs_booting(struct xregs_state *xstate)
{
	u64 mask = -1;
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	WARN_ON(system_state != SYSTEM_BOOTING);

	if (boot_cpu_has(X86_FEATURE_XSAVES))
		XSTATE_OP(XRSTORS, xstate, lmask, hmask, err);
	else
		XSTATE_OP(XRSTOR, xstate, lmask, hmask, err);

	/*
	 * We should never fault when copying from a kernel buffer, and the FPU
	 * state we set at boot time should be valid.
	 */
	WARN_ON_FPU(err);
}