static int blk_cloned_rq_check_limits(struct request_queue *q,
				      struct request *rq)
{
	if (blk_rq_sectors(rq) > blk_queue_get_max_sectors(q, req_op(rq))) {
		printk(KERN_ERR "%s: over max size limit. (%u > %u)\n",
			__func__, blk_rq_sectors(rq),
			blk_queue_get_max_sectors(q, req_op(rq)));
		return -EIO;
	}

	/*
	 * queue's settings related to segment counting like q->bounce_pfn
	 * may differ from that of other stacking queues.
	 * Recalculate it to check the request correctly on this queue's
	 * limitation.
	 */
	blk_recalc_rq_segments(rq);
	if (rq->nr_phys_segments > queue_max_segments(q)) {
		printk(KERN_ERR "%s: over max segments limit. (%hu > %hu)\n",
			__func__, rq->nr_phys_segments, queue_max_segments(q));
		return -EIO;
	}

	return 0;
}