void bnx2x_free_mem_cnic(struct bnx2x *bp)
{
	bnx2x_ilt_mem_op_cnic(bp, ILT_MEMOP_FREE);

	if (!CHIP_IS_E1x(bp))
		BNX2X_PCI_FREE(bp->cnic_sb.e2_sb, bp->cnic_sb_mapping,
			       sizeof(struct host_hc_status_block_e2));
	else
		BNX2X_PCI_FREE(bp->cnic_sb.e1x_sb, bp->cnic_sb_mapping,
			       sizeof(struct host_hc_status_block_e1x));

	BNX2X_PCI_FREE(bp->t2, bp->t2_mapping, SRC_T2_SZ);
}