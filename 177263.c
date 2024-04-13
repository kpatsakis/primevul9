int sdma_send_txreq(struct sdma_engine *sde,
		    struct iowait_work *wait,
		    struct sdma_txreq *tx,
		    bool pkts_sent)
{
	int ret = 0;
	u16 tail;
	unsigned long flags;

	/* user should have supplied entire packet */
	if (unlikely(tx->tlen))
		return -EINVAL;
	tx->wait = iowait_ioww_to_iow(wait);
	spin_lock_irqsave(&sde->tail_lock, flags);
retry:
	if (unlikely(!__sdma_running(sde)))
		goto unlock_noconn;
	if (unlikely(tx->num_desc > sde->desc_avail))
		goto nodesc;
	tail = submit_tx(sde, tx);
	if (wait)
		iowait_sdma_inc(iowait_ioww_to_iow(wait));
	sdma_update_tail(sde, tail);
unlock:
	spin_unlock_irqrestore(&sde->tail_lock, flags);
	return ret;
unlock_noconn:
	if (wait)
		iowait_sdma_inc(iowait_ioww_to_iow(wait));
	tx->next_descq_idx = 0;
#ifdef CONFIG_HFI1_DEBUG_SDMA_ORDER
	tx->sn = sde->tail_sn++;
	trace_hfi1_sdma_in_sn(sde, tx->sn);
#endif
	spin_lock(&sde->flushlist_lock);
	list_add_tail(&tx->list, &sde->flushlist);
	spin_unlock(&sde->flushlist_lock);
	iowait_inc_wait_count(wait, tx->num_desc);
	queue_work_on(sde->cpu, system_highpri_wq, &sde->flush_worker);
	ret = -ECOMM;
	goto unlock;
nodesc:
	ret = sdma_check_progress(sde, wait, tx, pkts_sent);
	if (ret == -EAGAIN) {
		ret = 0;
		goto retry;
	}
	sde->descq_full_count++;
	goto unlock;
}