static void __blk_release_queue(struct work_struct *work)
{
	struct request_queue *q = container_of(work, typeof(*q), release_work);

	if (test_bit(QUEUE_FLAG_POLL_STATS, &q->queue_flags))
		blk_stat_remove_callback(q, q->poll_cb);
	blk_stat_free_callback(q->poll_cb);

	blk_free_queue_stats(q->stats);

	blk_exit_queue(q);

	blk_queue_free_zone_bitmaps(q);

	if (queue_is_mq(q))
		blk_mq_release(q);

	blk_trace_shutdown(q);

	if (queue_is_mq(q))
		blk_mq_debugfs_unregister(q);

	bioset_exit(&q->bio_split);

	ida_simple_remove(&blk_queue_ida, q->id);
	call_rcu(&q->rcu_head, blk_free_queue_rcu);
}