void bnx2x_free_mem(struct bnx2x *bp)
{
	int i;

	BNX2X_PCI_FREE(bp->fw_stats, bp->fw_stats_mapping,
		       bp->fw_stats_data_sz + bp->fw_stats_req_sz);

	if (IS_VF(bp))
		return;

	BNX2X_PCI_FREE(bp->def_status_blk, bp->def_status_blk_mapping,
		       sizeof(struct host_sp_status_block));

	BNX2X_PCI_FREE(bp->slowpath, bp->slowpath_mapping,
		       sizeof(struct bnx2x_slowpath));

	for (i = 0; i < L2_ILT_LINES(bp); i++)
		BNX2X_PCI_FREE(bp->context[i].vcxt, bp->context[i].cxt_mapping,
			       bp->context[i].size);
	bnx2x_ilt_mem_op(bp, ILT_MEMOP_FREE);

	BNX2X_FREE(bp->ilt->lines);

	BNX2X_PCI_FREE(bp->spq, bp->spq_mapping, BCM_PAGE_SIZE);

	BNX2X_PCI_FREE(bp->eq_ring, bp->eq_mapping,
		       BCM_PAGE_SIZE * NUM_EQ_PAGES);

	BNX2X_PCI_FREE(bp->t2, bp->t2_mapping, SRC_T2_SZ);

	bnx2x_iov_free_mem(bp);
}