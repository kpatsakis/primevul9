static u16 bnx2x_update_dsb_idx(struct bnx2x *bp)
{
	struct host_sp_status_block *def_sb = bp->def_status_blk;
	u16 rc = 0;

	barrier(); /* status block is written to by the chip */
	if (bp->def_att_idx != def_sb->atten_status_block.attn_bits_index) {
		bp->def_att_idx = def_sb->atten_status_block.attn_bits_index;
		rc |= BNX2X_DEF_SB_ATT_IDX;
	}

	if (bp->def_idx != def_sb->sp_sb.running_index) {
		bp->def_idx = def_sb->sp_sb.running_index;
		rc |= BNX2X_DEF_SB_IDX;
	}

	/* Do not reorder: indices reading should complete before handling */
	barrier();
	return rc;
}