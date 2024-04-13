static inline int copy_kernel_to_xregs_err(struct xregs_state *xstate, u64 mask)
{
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	XSTATE_OP(XRSTOR, xstate, lmask, hmask, err);

	return err;
}