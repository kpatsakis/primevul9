void blk_mq_sched_restart(struct blk_mq_hw_ctx *hctx)
{
	if (!test_bit(BLK_MQ_S_SCHED_RESTART, &hctx->state))
		return;
	clear_bit(BLK_MQ_S_SCHED_RESTART, &hctx->state);

	blk_mq_run_hw_queue(hctx, true);
}