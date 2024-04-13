void bnx2x_write_dmae(struct bnx2x *bp, dma_addr_t dma_addr, u32 dst_addr,
		      u32 len32)
{
	int rc;
	struct dmae_command dmae;

	if (!bp->dmae_ready) {
		u32 *data = bnx2x_sp(bp, wb_data[0]);

		if (CHIP_IS_E1(bp))
			bnx2x_init_ind_wr(bp, dst_addr, data, len32);
		else
			bnx2x_init_str_wr(bp, dst_addr, data, len32);
		return;
	}

	/* set opcode and fixed command fields */
	bnx2x_prep_dmae_with_comp(bp, &dmae, DMAE_SRC_PCI, DMAE_DST_GRC);

	/* fill in addresses and len */
	dmae.src_addr_lo = U64_LO(dma_addr);
	dmae.src_addr_hi = U64_HI(dma_addr);
	dmae.dst_addr_lo = dst_addr >> 2;
	dmae.dst_addr_hi = 0;
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