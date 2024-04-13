static void validate_set_si_mode(struct bnx2x *bp)
{
	u8 func = BP_ABS_FUNC(bp);
	u32 val;

	val = MF_CFG_RD(bp, func_mf_config[func].mac_upper);

	/* check for legal mac (upper bytes) */
	if (val != 0xffff) {
		bp->mf_mode = MULTI_FUNCTION_SI;
		bp->mf_config[BP_VN(bp)] =
			MF_CFG_RD(bp, func_mf_config[func].config);
	} else
		BNX2X_DEV_INFO("illegal MAC address for SI\n");
}