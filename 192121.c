static int mce_cpu_online(unsigned int cpu)
{
	struct timer_list *t = this_cpu_ptr(&mce_timer);
	int ret;

	mce_device_create(cpu);

	ret = mce_threshold_create_device(cpu);
	if (ret) {
		mce_device_remove(cpu);
		return ret;
	}
	mce_reenable_cpu();
	mce_start_timer(t);
	return 0;
}