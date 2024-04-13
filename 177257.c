static void sdma_desc_avail(struct sdma_engine *sde, uint avail)
{
	struct iowait *wait, *nw, *twait;
	struct iowait *waits[SDMA_WAIT_BATCH_SIZE];
	uint i, n = 0, seq, tidx = 0;

#ifdef CONFIG_SDMA_VERBOSITY
	dd_dev_err(sde->dd, "CONFIG SDMA(%u) %s:%d %s()\n", sde->this_idx,
		   slashstrip(__FILE__), __LINE__, __func__);
	dd_dev_err(sde->dd, "avail: %u\n", avail);
#endif

	do {
		seq = read_seqbegin(&sde->waitlock);
		if (!list_empty(&sde->dmawait)) {
			/* at least one item */
			write_seqlock(&sde->waitlock);
			/* Harvest waiters wanting DMA descriptors */
			list_for_each_entry_safe(
					wait,
					nw,
					&sde->dmawait,
					list) {
				u32 num_desc;

				if (!wait->wakeup)
					continue;
				if (n == ARRAY_SIZE(waits))
					break;
				iowait_init_priority(wait);
				num_desc = iowait_get_all_desc(wait);
				if (num_desc > avail)
					break;
				avail -= num_desc;
				/* Find the top-priority wait memeber */
				if (n) {
					twait = waits[tidx];
					tidx =
					    iowait_priority_update_top(wait,
								       twait,
								       n,
								       tidx);
				}
				list_del_init(&wait->list);
				waits[n++] = wait;
			}
			write_sequnlock(&sde->waitlock);
			break;
		}
	} while (read_seqretry(&sde->waitlock, seq));

	/* Schedule the top-priority entry first */
	if (n)
		waits[tidx]->wakeup(waits[tidx], SDMA_AVAIL_REASON);

	for (i = 0; i < n; i++)
		if (i != tidx)
			waits[i]->wakeup(waits[i], SDMA_AVAIL_REASON);
}