int bnx2x_alloc_mem(struct bnx2x *bp)
{
	int i, allocated, context_size;

	if (!CONFIGURE_NIC_MODE(bp) && !bp->t2) {
		/* allocate searcher T2 table */
		bp->t2 = BNX2X_PCI_ALLOC(&bp->t2_mapping, SRC_T2_SZ);
		if (!bp->t2)
			goto alloc_mem_err;
	}

	bp->def_status_blk = BNX2X_PCI_ALLOC(&bp->def_status_blk_mapping,
					     sizeof(struct host_sp_status_block));
	if (!bp->def_status_blk)
		goto alloc_mem_err;

	bp->slowpath = BNX2X_PCI_ALLOC(&bp->slowpath_mapping,
				       sizeof(struct bnx2x_slowpath));
	if (!bp->slowpath)
		goto alloc_mem_err;

	/* Allocate memory for CDU context:
	 * This memory is allocated separately and not in the generic ILT
	 * functions because CDU differs in few aspects:
	 * 1. There are multiple entities allocating memory for context -
	 * 'regular' driver, CNIC and SRIOV driver. Each separately controls
	 * its own ILT lines.
	 * 2. Since CDU page-size is not a single 4KB page (which is the case
	 * for the other ILT clients), to be efficient we want to support
	 * allocation of sub-page-size in the last entry.
	 * 3. Context pointers are used by the driver to pass to FW / update
	 * the context (for the other ILT clients the pointers are used just to
	 * free the memory during unload).
	 */
	context_size = sizeof(union cdu_context) * BNX2X_L2_CID_COUNT(bp);

	for (i = 0, allocated = 0; allocated < context_size; i++) {
		bp->context[i].size = min(CDU_ILT_PAGE_SZ,
					  (context_size - allocated));
		bp->context[i].vcxt = BNX2X_PCI_ALLOC(&bp->context[i].cxt_mapping,
						      bp->context[i].size);
		if (!bp->context[i].vcxt)
			goto alloc_mem_err;
		allocated += bp->context[i].size;
	}
	bp->ilt->lines = kcalloc(ILT_MAX_LINES, sizeof(struct ilt_line),
				 GFP_KERNEL);
	if (!bp->ilt->lines)
		goto alloc_mem_err;

	if (bnx2x_ilt_mem_op(bp, ILT_MEMOP_ALLOC))
		goto alloc_mem_err;

	if (bnx2x_iov_alloc_mem(bp))
		goto alloc_mem_err;

	/* Slow path ring */
	bp->spq = BNX2X_PCI_ALLOC(&bp->spq_mapping, BCM_PAGE_SIZE);
	if (!bp->spq)
		goto alloc_mem_err;

	/* EQ */
	bp->eq_ring = BNX2X_PCI_ALLOC(&bp->eq_mapping,
				      BCM_PAGE_SIZE * NUM_EQ_PAGES);
	if (!bp->eq_ring)
		goto alloc_mem_err;

	return 0;

alloc_mem_err:
	bnx2x_free_mem(bp);
	BNX2X_ERR("Can't allocate memory\n");
	return -ENOMEM;
}