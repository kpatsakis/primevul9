static void sdma_process_event(struct sdma_engine *sde, enum sdma_events event)
{
	unsigned long flags;

	spin_lock_irqsave(&sde->tail_lock, flags);
	write_seqlock(&sde->head_lock);

	__sdma_process_event(sde, event);

	if (sde->state.current_state == sdma_state_s99_running)
		sdma_desc_avail(sde, sdma_descq_freecnt(sde));

	write_sequnlock(&sde->head_lock);
	spin_unlock_irqrestore(&sde->tail_lock, flags);
}