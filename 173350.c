static void storm_memset_vf_to_pf(struct bnx2x *bp, u16 abs_fid,
				  u16 pf_id)
{
	REG_WR8(bp, BAR_XSTRORM_INTMEM + XSTORM_VF_TO_PF_OFFSET(abs_fid),
		pf_id);
	REG_WR8(bp, BAR_CSTRORM_INTMEM + CSTORM_VF_TO_PF_OFFSET(abs_fid),
		pf_id);
	REG_WR8(bp, BAR_TSTRORM_INTMEM + TSTORM_VF_TO_PF_OFFSET(abs_fid),
		pf_id);
	REG_WR8(bp, BAR_USTRORM_INTMEM + USTORM_VF_TO_PF_OFFSET(abs_fid),
		pf_id);
}