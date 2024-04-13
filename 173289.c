static void bnx2x_igu_clear_sb(struct bnx2x *bp, u8 idu_sb_id)
{
	bnx2x_igu_clear_sb_gen(bp, BP_FUNC(bp), idu_sb_id, true /*PF*/);
}