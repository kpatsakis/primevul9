bool bnx2x_reset_is_done(struct bnx2x *bp, int engine)
{
	u32 val = REG_RD(bp, BNX2X_RECOVERY_GLOB_REG);
	u32 bit = engine ?
		BNX2X_PATH1_RST_IN_PROG_BIT : BNX2X_PATH0_RST_IN_PROG_BIT;

	/* return false if bit is set */
	return (val & bit) ? false : true;
}