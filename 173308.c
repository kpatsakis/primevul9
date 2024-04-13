static void bnx2x_pxp_prep(struct bnx2x *bp)
{
	if (!CHIP_IS_E1(bp)) {
		REG_WR(bp, PXP2_REG_RD_START_INIT, 0);
		REG_WR(bp, PXP2_REG_RQ_RBC_DONE, 0);
		mmiowb();
	}
}