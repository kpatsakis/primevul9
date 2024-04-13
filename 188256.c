void temac_indirect_out32_locked(struct temac_local *lp, int reg, u32 value)
{
	/* As in temac_indirect_in32_locked(), we should normally not
	 * spin here.  And if it happens, we actually end up silently
	 * ignoring the write request.  Ouch.
	 */
	if (WARN_ON(temac_indirect_busywait(lp)))
		return;
	/* Initiate write to indirect register */
	temac_iow(lp, XTE_LSW0_OFFSET, value);
	temac_iow(lp, XTE_CTL0_OFFSET, CNTLREG_WRITE_ENABLE_MASK | reg);
	/* As in temac_indirect_in32_locked(), we should not see timeouts
	 * here.  And if it happens, we continue before the write has
	 * completed.  Not good.
	 */
	WARN_ON(temac_indirect_busywait(lp));
}