static int mce_cpu_pre_down(unsigned int cpu)
{
	struct timer_list *t = this_cpu_ptr(&mce_timer);

	mce_disable_cpu();
	del_timer_sync(t);
	mce_threshold_remove_device(cpu);
	mce_device_remove(cpu);
	return 0;
}