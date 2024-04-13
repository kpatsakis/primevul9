static u32 bnx2x_rx_ustorm_prods_offset(struct bnx2x_fastpath *fp)
{
	struct bnx2x *bp = fp->bp;
	u32 offset = BAR_USTRORM_INTMEM;

	if (IS_VF(bp))
		return bnx2x_vf_ustorm_prods_offset(bp, fp);
	else if (!CHIP_IS_E1x(bp))
		offset += USTORM_RX_PRODS_E2_OFFSET(fp->cl_qzone_id);
	else
		offset += USTORM_RX_PRODS_E1X_OFFSET(BP_PORT(bp), fp->cl_id);

	return offset;
}