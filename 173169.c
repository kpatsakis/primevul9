void bnx2x_pre_irq_nic_init(struct bnx2x *bp)
{
	int i;

	/* Setup NIC internals and enable interrupts */
	for_each_eth_queue(bp, i)
		bnx2x_init_eth_fp(bp, i);

	/* ensure status block indices were read */
	rmb();
	bnx2x_init_rx_rings(bp);
	bnx2x_init_tx_rings(bp);

	if (IS_PF(bp)) {
		/* Initialize MOD_ABS interrupts */
		bnx2x_init_mod_abs_int(bp, &bp->link_vars, bp->common.chip_id,
				       bp->common.shmem_base,
				       bp->common.shmem2_base, BP_PORT(bp));

		/* initialize the default status block and sp ring */
		bnx2x_init_def_sb(bp);
		bnx2x_update_dsb_idx(bp);
		bnx2x_init_sp_ring(bp);
	} else {
		bnx2x_memset_stats(bp);
	}
}