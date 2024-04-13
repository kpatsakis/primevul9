void bnx2x_int_disable_sync(struct bnx2x *bp, int disable_hw)
{
	int msix = (bp->flags & USING_MSIX_FLAG) ? 1 : 0;
	int i, offset;

	if (disable_hw)
		/* prevent the HW from sending interrupts */
		bnx2x_int_disable(bp);

	/* make sure all ISRs are done */
	if (msix) {
		synchronize_irq(bp->msix_table[0].vector);
		offset = 1;
		if (CNIC_SUPPORT(bp))
			offset++;
		for_each_eth_queue(bp, i)
			synchronize_irq(bp->msix_table[offset++].vector);
	} else
		synchronize_irq(bp->pdev->irq);

	/* make sure sp_task is not running */
	cancel_delayed_work(&bp->sp_task);
	cancel_delayed_work(&bp->period_task);
	flush_workqueue(bnx2x_wq);
}