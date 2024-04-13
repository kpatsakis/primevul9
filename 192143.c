static void mce_restart(void)
{
	mce_timer_delete_all();
	on_each_cpu(mce_cpu_restart, NULL, 1);
}