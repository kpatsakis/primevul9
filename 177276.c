int sdma_send_txlist(struct sdma_engine *sde, struct iowait_work *wait,
		     struct list_head *tx_list, u16 *count_out)
{
	struct sdma_txreq *tx, *tx_next;
	int ret = 0;
	unsigned long flags;
	u16 tail = INVALID_TAIL;
	u32 submit_count = 0, flush_count = 0, total_count;

	spin_lock_irqsave(&sde->tail_lock, flags);
retry:
	list_for_each_entry_safe(tx, tx_next, tx_list, list) {
		tx->wait = iowait_ioww_to_iow(wait);
		if (unlikely(!__sdma_running(sde)))
			goto unlock_noconn;
		if (unlikely(tx->num_desc > sde->desc_avail))
			goto nodesc;
		if (unlikely(tx->tlen)) {
			ret = -EINVAL;
			goto update_tail;
		}
		list_del_init(&tx->list);
		tail = submit_tx(sde, tx);
		submit_count++;
		if (tail != INVALID_TAIL &&
		    (submit_count & SDMA_TAIL_UPDATE_THRESH) == 0) {
			sdma_update_tail(sde, tail);
			tail = INVALID_TAIL;
		}
	}
update_tail:
	total_count = submit_count + flush_count;
	if (wait) {
		iowait_sdma_add(iowait_ioww_to_iow(wait), total_count);
		iowait_starve_clear(submit_count > 0,
				    iowait_ioww_to_iow(wait));
	}
	if (tail != INVALID_TAIL)
		sdma_update_tail(sde, tail);
	spin_unlock_irqrestore(&sde->tail_lock, flags);
	*count_out = total_count;
	return ret;
unlock_noconn:
	spin_lock(&sde->flushlist_lock);
	list_for_each_entry_safe(tx, tx_next, tx_list, list) {
		tx->wait = iowait_ioww_to_iow(wait);
		list_del_init(&tx->list);
		tx->next_descq_idx = 0;
#ifdef CONFIG_HFI1_DEBUG_SDMA_ORDER
		tx->sn = sde->tail_sn++;
		trace_hfi1_sdma_in_sn(sde, tx->sn);
#endif
		list_add_tail(&tx->list, &sde->flushlist);
		flush_count++;
		iowait_inc_wait_count(wait, tx->num_desc);
	}
	spin_unlock(&sde->flushlist_lock);
	queue_work_on(sde->cpu, system_highpri_wq, &sde->flush_worker);
	ret = -ECOMM;
	goto update_tail;
nodesc:
	ret = sdma_check_progress(sde, wait, tx, submit_count > 0);
	if (ret == -EAGAIN) {
		ret = 0;
		goto retry;
	}
	sde->descq_full_count++;
	goto update_tail;
}