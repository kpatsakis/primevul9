static void bnx2x_pf_q_prep_general(struct bnx2x *bp,
	struct bnx2x_fastpath *fp, struct bnx2x_general_setup_params *gen_init,
	u8 cos)
{
	gen_init->stat_id = bnx2x_stats_id(fp);
	gen_init->spcl_id = fp->cl_id;

	/* Always use mini-jumbo MTU for FCoE L2 ring */
	if (IS_FCOE_FP(fp))
		gen_init->mtu = BNX2X_FCOE_MINI_JUMBO_MTU;
	else
		gen_init->mtu = bp->dev->mtu;

	gen_init->cos = cos;

	gen_init->fp_hsi = ETH_FP_HSI_VERSION;
}