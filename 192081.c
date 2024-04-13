static void mce_irq_work_cb(struct irq_work *entry)
{
	mce_schedule_work();
}