static inline void copy_xregs_to_kernel_booting(struct xregs_state *xstate)
{
	u64 mask = -1;
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	WARN_ON(system_state != SYSTEM_BOOTING);

	if (boot_cpu_has(X86_FEATURE_XSAVES))
		XSTATE_OP(XSAVES, xstate, lmask, hmask, err);
	else
		XSTATE_OP(XSAVE, xstate, lmask, hmask, err);

	/* We should never fault when copying to a kernel buffer: */
	WARN_ON_FPU(err);
}