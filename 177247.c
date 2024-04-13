static int sdma_check_progress(
	struct sdma_engine *sde,
	struct iowait_work *wait,
	struct sdma_txreq *tx,
	bool pkts_sent)
{
	int ret;

	sde->desc_avail = sdma_descq_freecnt(sde);
	if (tx->num_desc <= sde->desc_avail)
		return -EAGAIN;
	/* pulse the head_lock */
	if (wait && iowait_ioww_to_iow(wait)->sleep) {
		unsigned seq;

		seq = raw_seqcount_begin(
			(const seqcount_t *)&sde->head_lock.seqcount);
		ret = wait->iow->sleep(sde, wait, tx, seq, pkts_sent);
		if (ret == -EAGAIN)
			sde->desc_avail = sdma_descq_freecnt(sde);
	} else {
		ret = -EBUSY;
	}
	return ret;
}