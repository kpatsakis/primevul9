static inline int copy_user_to_xregs(struct xregs_state __user *buf, u64 mask)
{
	struct xregs_state *xstate = ((__force struct xregs_state *)buf);
	u32 lmask = mask;
	u32 hmask = mask >> 32;
	int err;

	stac();
	XSTATE_OP(XRSTOR, xstate, lmask, hmask, err);
	clac();

	return err;
}