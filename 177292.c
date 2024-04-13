static void sdma_sw_clean_up_task(unsigned long opaque)
{
	struct sdma_engine *sde = (struct sdma_engine *)opaque;
	unsigned long flags;

	spin_lock_irqsave(&sde->tail_lock, flags);
	write_seqlock(&sde->head_lock);

	/*
	 * At this point, the following should always be true:
	 * - We are halted, so no more descriptors are getting retired.
	 * - We are not running, so no one is submitting new work.
	 * - Only we can send the e40_sw_cleaned, so we can't start
	 *   running again until we say so.  So, the active list and
	 *   descq are ours to play with.
	 */

	/*
	 * In the error clean up sequence, software clean must be called
	 * before the hardware clean so we can use the hardware head in
	 * the progress routine.  A hardware clean or SPC unfreeze will
	 * reset the hardware head.
	 *
	 * Process all retired requests. The progress routine will use the
	 * latest physical hardware head - we are not running so speed does
	 * not matter.
	 */
	sdma_make_progress(sde, 0);

	sdma_flush(sde);

	/*
	 * Reset our notion of head and tail.
	 * Note that the HW registers have been reset via an earlier
	 * clean up.
	 */
	sde->descq_tail = 0;
	sde->descq_head = 0;
	sde->desc_avail = sdma_descq_freecnt(sde);
	*sde->head_dma = 0;

	__sdma_process_event(sde, sdma_event_e40_sw_cleaned);

	write_sequnlock(&sde->head_lock);
	spin_unlock_irqrestore(&sde->tail_lock, flags);
}