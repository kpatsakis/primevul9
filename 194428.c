static inline void fpstate_init_xstate(struct xregs_state *xsave)
{
	/*
	 * XRSTORS requires these bits set in xcomp_bv, or it will
	 * trigger #GP:
	 */
	xsave->header.xcomp_bv = XCOMP_BV_COMPACTED_FORMAT | xfeatures_mask;
}