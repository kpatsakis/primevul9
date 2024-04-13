static void bnx2x_init_def_sb(struct bnx2x *bp)
{
	struct host_sp_status_block *def_sb = bp->def_status_blk;
	dma_addr_t mapping = bp->def_status_blk_mapping;
	int igu_sp_sb_index;
	int igu_seg_id;
	int port = BP_PORT(bp);
	int func = BP_FUNC(bp);
	int reg_offset, reg_offset_en5;
	u64 section;
	int index;
	struct hc_sp_status_block_data sp_sb_data;
	memset(&sp_sb_data, 0, sizeof(struct hc_sp_status_block_data));

	if (CHIP_INT_MODE_IS_BC(bp)) {
		igu_sp_sb_index = DEF_SB_IGU_ID;
		igu_seg_id = HC_SEG_ACCESS_DEF;
	} else {
		igu_sp_sb_index = bp->igu_dsb_id;
		igu_seg_id = IGU_SEG_ACCESS_DEF;
	}

	/* ATTN */
	section = ((u64)mapping) + offsetof(struct host_sp_status_block,
					    atten_status_block);
	def_sb->atten_status_block.status_block_id = igu_sp_sb_index;

	bp->attn_state = 0;

	reg_offset = (port ? MISC_REG_AEU_ENABLE1_FUNC_1_OUT_0 :
			     MISC_REG_AEU_ENABLE1_FUNC_0_OUT_0);
	reg_offset_en5 = (port ? MISC_REG_AEU_ENABLE5_FUNC_1_OUT_0 :
				 MISC_REG_AEU_ENABLE5_FUNC_0_OUT_0);
	for (index = 0; index < MAX_DYNAMIC_ATTN_GRPS; index++) {
		int sindex;
		/* take care of sig[0]..sig[4] */
		for (sindex = 0; sindex < 4; sindex++)
			bp->attn_group[index].sig[sindex] =
			   REG_RD(bp, reg_offset + sindex*0x4 + 0x10*index);

		if (!CHIP_IS_E1x(bp))
			/*
			 * enable5 is separate from the rest of the registers,
			 * and therefore the address skip is 4
			 * and not 16 between the different groups
			 */
			bp->attn_group[index].sig[4] = REG_RD(bp,
					reg_offset_en5 + 0x4*index);
		else
			bp->attn_group[index].sig[4] = 0;
	}

	if (bp->common.int_block == INT_BLOCK_HC) {
		reg_offset = (port ? HC_REG_ATTN_MSG1_ADDR_L :
				     HC_REG_ATTN_MSG0_ADDR_L);

		REG_WR(bp, reg_offset, U64_LO(section));
		REG_WR(bp, reg_offset + 4, U64_HI(section));
	} else if (!CHIP_IS_E1x(bp)) {
		REG_WR(bp, IGU_REG_ATTN_MSG_ADDR_L, U64_LO(section));
		REG_WR(bp, IGU_REG_ATTN_MSG_ADDR_H, U64_HI(section));
	}

	section = ((u64)mapping) + offsetof(struct host_sp_status_block,
					    sp_sb);

	bnx2x_zero_sp_sb(bp);

	/* PCI guarantees endianity of regpairs */
	sp_sb_data.state		= SB_ENABLED;
	sp_sb_data.host_sb_addr.lo	= U64_LO(section);
	sp_sb_data.host_sb_addr.hi	= U64_HI(section);
	sp_sb_data.igu_sb_id		= igu_sp_sb_index;
	sp_sb_data.igu_seg_id		= igu_seg_id;
	sp_sb_data.p_func.pf_id		= func;
	sp_sb_data.p_func.vnic_id	= BP_VN(bp);
	sp_sb_data.p_func.vf_id		= 0xff;

	bnx2x_wr_sp_sb_data(bp, &sp_sb_data);

	bnx2x_ack_sb(bp, bp->igu_dsb_id, USTORM_ID, 0, IGU_INT_ENABLE, 0);
}