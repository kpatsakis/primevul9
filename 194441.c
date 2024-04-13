static inline int copy_xregs_to_user(struct xregs_state __user *buf)
{
	int err;

	/*
	 * Clear the xsave header first, so that reserved fields are
	 * initialized to zero.
	 */
	err = __clear_user(&buf->header, sizeof(buf->header));
	if (unlikely(err))
		return -EFAULT;

	stac();
	XSTATE_OP(XSAVE, buf, -1, -1, err);
	clac();

	return err;
}