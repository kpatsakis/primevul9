void bnx2x_igu_ack_sb(struct bnx2x *bp, u8 igu_sb_id, u8 segment,
		      u16 index, u8 op, u8 update)
{
	u32 igu_addr = bp->igu_base_addr;
	igu_addr += (IGU_CMD_INT_ACK_BASE + igu_sb_id)*8;
	bnx2x_igu_ack_sb_gen(bp, igu_sb_id, segment, index, op, update,
			     igu_addr);
}