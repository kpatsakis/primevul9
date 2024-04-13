int kblockd_schedule_work_on(int cpu, struct work_struct *work)
{
	return queue_work_on(cpu, kblockd_workqueue, work);
}