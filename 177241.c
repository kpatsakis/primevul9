static void sdma_field_flush(struct work_struct *work)
{
	unsigned long flags;
	struct sdma_engine *sde =
		container_of(work, struct sdma_engine, flush_worker);

	write_seqlock_irqsave(&sde->head_lock, flags);
	if (!__sdma_running(sde))
		sdma_flush(sde);
	write_sequnlock_irqrestore(&sde->head_lock, flags);
}