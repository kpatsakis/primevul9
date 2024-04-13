static void drain_local_pages_wq(struct work_struct *work)
{
	/*
	 * drain_all_pages doesn't use proper cpu hotplug protection so
	 * we can race with cpu offline when the WQ can move this from
	 * a cpu pinned worker to an unbound one. We can operate on a different
	 * cpu which is allright but we also have to make sure to not move to
	 * a different one.
	 */
	preempt_disable();
	drain_local_pages(NULL);
	preempt_enable();
}