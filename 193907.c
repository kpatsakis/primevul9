int kblockd_mod_delayed_work_on(int cpu, struct delayed_work *dwork,
				unsigned long delay)
{
	return mod_delayed_work_on(cpu, kblockd_workqueue, dwork, delay);
}