static int bnx2x_get_cmng_fns_mode(struct bnx2x *bp)
{
	if (CHIP_REV_IS_SLOW(bp))
		return CMNG_FNS_NONE;
	if (IS_MF(bp))
		return CMNG_FNS_MINMAX;

	return CMNG_FNS_NONE;
}