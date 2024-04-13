static void sdma_err_progress_check(struct timer_list *t)
{
	unsigned index;
	struct sdma_engine *sde = from_timer(sde, t, err_progress_check_timer);

	dd_dev_err(sde->dd, "SDE progress check event\n");
	for (index = 0; index < sde->dd->num_sdma; index++) {
		struct sdma_engine *curr_sde = &sde->dd->per_sdma[index];
		unsigned long flags;

		/* check progress on each engine except the current one */
		if (curr_sde == sde)
			continue;
		/*
		 * We must lock interrupts when acquiring sde->lock,
		 * to avoid a deadlock if interrupt triggers and spins on
		 * the same lock on same CPU
		 */
		spin_lock_irqsave(&curr_sde->tail_lock, flags);
		write_seqlock(&curr_sde->head_lock);

		/* skip non-running queues */
		if (curr_sde->state.current_state != sdma_state_s99_running) {
			write_sequnlock(&curr_sde->head_lock);
			spin_unlock_irqrestore(&curr_sde->tail_lock, flags);
			continue;
		}

		if ((curr_sde->descq_head != curr_sde->descq_tail) &&
		    (curr_sde->descq_head ==
				curr_sde->progress_check_head))
			__sdma_process_event(curr_sde,
					     sdma_event_e90_sw_halted);
		write_sequnlock(&curr_sde->head_lock);
		spin_unlock_irqrestore(&curr_sde->tail_lock, flags);
	}
	schedule_work(&sde->err_halt_worker);
}