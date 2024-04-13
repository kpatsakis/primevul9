u32 temac_indirect_in32_locked(struct temac_local *lp, int reg)
{
	/* This initial wait should normally not spin, as we always
	 * try to wait for indirect access to complete before
	 * releasing the indirect_lock.
	 */
	if (WARN_ON(temac_indirect_busywait(lp)))
		return -ETIMEDOUT;
	/* Initiate read from indirect register */
	temac_iow(lp, XTE_CTL0_OFFSET, reg);
	/* Wait for indirect register access to complete.  We really
	 * should not see timeouts, and could even end up causing
	 * problem for following indirect access, so let's make a bit
	 * of WARN noise.
	 */
	if (WARN_ON(temac_indirect_busywait(lp)))
		return -ETIMEDOUT;
	/* Value is ready now */
	return temac_ior(lp, XTE_LSW0_OFFSET);
}