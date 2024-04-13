static inline void copy_kernel_to_xregs(struct xregs_state *xstate, u64 mask)
{
	u32 lmask = mask;
	u32 hmask = mask >> 32;

	XSTATE_XRESTORE(xstate, lmask, hmask);
}