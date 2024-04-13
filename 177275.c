static void sdma_make_progress(struct sdma_engine *sde, u64 status)
{
	struct sdma_txreq *txp = NULL;
	int progress = 0;
	u16 hwhead, swhead;
	int idle_check_done = 0;

	hwhead = sdma_gethead(sde);

	/* The reason for some of the complexity of this code is that
	 * not all descriptors have corresponding txps.  So, we have to
	 * be able to skip over descs until we wander into the range of
	 * the next txp on the list.
	 */

retry:
	txp = get_txhead(sde);
	swhead = sde->descq_head & sde->sdma_mask;
	trace_hfi1_sdma_progress(sde, hwhead, swhead, txp);
	while (swhead != hwhead) {
		/* advance head, wrap if needed */
		swhead = ++sde->descq_head & sde->sdma_mask;

		/* if now past this txp's descs, do the callback */
		if (txp && txp->next_descq_idx == swhead) {
			/* remove from list */
			sde->tx_ring[sde->tx_head++ & sde->sdma_mask] = NULL;
			complete_tx(sde, txp, SDMA_TXREQ_S_OK);
			/* see if there is another txp */
			txp = get_txhead(sde);
		}
		trace_hfi1_sdma_progress(sde, hwhead, swhead, txp);
		progress++;
	}

	/*
	 * The SDMA idle interrupt is not guaranteed to be ordered with respect
	 * to updates to the the dma_head location in host memory. The head
	 * value read might not be fully up to date. If there are pending
	 * descriptors and the SDMA idle interrupt fired then read from the
	 * CSR SDMA head instead to get the latest value from the hardware.
	 * The hardware SDMA head should be read at most once in this invocation
	 * of sdma_make_progress(..) which is ensured by idle_check_done flag
	 */
	if ((status & sde->idle_mask) && !idle_check_done) {
		u16 swtail;

		swtail = READ_ONCE(sde->descq_tail) & sde->sdma_mask;
		if (swtail != hwhead) {
			hwhead = (u16)read_sde_csr(sde, SD(HEAD));
			idle_check_done = 1;
			goto retry;
		}
	}

	sde->last_status = status;
	if (progress)
		sdma_desc_avail(sde, sdma_descq_freecnt(sde));
}