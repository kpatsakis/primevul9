static bool bnx2x_prev_is_after_undi(struct bnx2x *bp)
{
	/* UNDI marks its presence in DORQ -
	 * it initializes CID offset for normal bell to 0x7
	 */
	if (!(REG_RD(bp, MISC_REG_RESET_REG_1) &
	    MISC_REGISTERS_RESET_REG_1_RST_DORQ))
		return false;

	if (REG_RD(bp, DORQ_REG_NORM_CID_OFST) == 0x7) {
		BNX2X_DEV_INFO("UNDI previously loaded\n");
		return true;
	}

	return false;
}