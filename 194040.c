void blk_account_io_start(struct request *rq, bool new_io)
{
	struct hd_struct *part;
	int rw = rq_data_dir(rq);

	if (!blk_do_io_stat(rq))
		return;

	part_stat_lock();

	if (!new_io) {
		part = rq->part;
		part_stat_inc(part, merges[rw]);
	} else {
		part = disk_map_sector_rcu(rq->rq_disk, blk_rq_pos(rq));
		if (!hd_struct_try_get(part)) {
			/*
			 * The partition is already being removed,
			 * the request will be accounted on the disk only
			 *
			 * We take a reference on disk->part0 although that
			 * partition will never be deleted, so we can treat
			 * it as any other partition.
			 */
			part = &rq->rq_disk->part0;
			hd_struct_get(part);
		}
		part_inc_in_flight(rq->q, part, rw);
		rq->part = part;
	}

	update_io_ticks(part, jiffies);

	part_stat_unlock();
}