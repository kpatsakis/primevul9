static inline void complete_tx(struct sdma_engine *sde,
			       struct sdma_txreq *tx,
			       int res)
{
	/* protect against complete modifying */
	struct iowait *wait = tx->wait;
	callback_t complete = tx->complete;

#ifdef CONFIG_HFI1_DEBUG_SDMA_ORDER
	trace_hfi1_sdma_out_sn(sde, tx->sn);
	if (WARN_ON_ONCE(sde->head_sn != tx->sn))
		dd_dev_err(sde->dd, "expected %llu got %llu\n",
			   sde->head_sn, tx->sn);
	sde->head_sn++;
#endif
	__sdma_txclean(sde->dd, tx);
	if (complete)
		(*complete)(tx, res);
	if (iowait_sdma_dec(wait))
		iowait_drain_wakeup(wait);
}