void bnx2x_read_dmae(struct bnx2x *bp, u32 src_addr, u32 len32)
{
	int rc;
	struct dmae_command dmae;

	if (!bp->dmae_ready) {
		u32 *data = bnx2x_sp(bp, wb_data[0]);
		int i;

		if (CHIP_IS_E1(bp))
			for (i = 0; i < len32; i++)
				data[i] = bnx2x_reg_rd_ind(bp, src_addr + i*4);
		else
			for (i = 0; i < len32; i++)
				data[i] = REG_RD(bp, src_addr + i*4);

		return;
	}

	/* set opcode and fixed command fields */
	bnx2x_prep_dmae_with_comp(bp, &dmae, DMAE_SRC_GRC, DMAE_DST_PCI);

	/* fill in addresses and len */
	dmae.src_addr_lo = src_addr >> 2;
	dmae.src_addr_hi = 0;
	dmae.dst_addr_lo = U64_LO(bnx2x_sp_mapping(bp, wb_data));
	dmae.dst_addr_hi = U64_HI(bnx2x_sp_mapping(bp, wb_data));
	dmae.len = len32;

	/* issue the command and wait for completion */
	rc = bnx2x_issue_dmae_with_comp(bp, &dmae, bnx2x_sp(bp, wb_comp));
	if (rc) {
		BNX2X_ERR("DMAE returned failure %d\n", rc);
#ifdef BNX2X_STOP_ON_ERROR
		bnx2x_panic();
#endif
	}
}