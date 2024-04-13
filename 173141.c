static int bnx2x_schedule_sp_task(struct bnx2x *bp)
{
	/* Set the interrupt occurred bit for the sp-task to recognize it
	 * must ack the interrupt and transition according to the IGU
	 * state machine.
	 */
	atomic_set(&bp->interrupt_occurred, 1);

	/* The sp_task must execute only after this bit
	 * is set, otherwise we will get out of sync and miss all
	 * further interrupts. Hence, the barrier.
	 */
	smp_wmb();

	/* schedule sp_task to workqueue */
	return queue_delayed_work(bnx2x_wq, &bp->sp_task, 0);
}