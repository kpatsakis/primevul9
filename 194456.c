static inline void copy_xregs_to_kernel(struct xregs_state *xstate)
{
	u64 mask = -1;
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	WARN_ON_FPU(!alternatives_patched);

	XSTATE_XSAVE(xstate, lmask, hmask, err);

	/* We should never fault when copying to a kernel buffer: */
	WARN_ON_FPU(err);
}