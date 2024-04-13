void bnx2x_notify_link_changed(struct bnx2x *bp)
{
	REG_WR(bp, MISC_REG_AEU_GENERAL_ATTN_12 + BP_FUNC(bp)*sizeof(u32), 1);
}