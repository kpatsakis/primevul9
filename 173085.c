static void bnx2x_clp_reset_prep(struct bnx2x *bp, u32 *magic_val)
{
	/* Do some magic... */
	u32 val = MF_CFG_RD(bp, shared_mf_config.clp_mb);
	*magic_val = val & SHARED_MF_CLP_MAGIC;
	MF_CFG_WR(bp, shared_mf_config.clp_mb, val | SHARED_MF_CLP_MAGIC);
}