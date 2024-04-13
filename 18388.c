static void timer_update_keys(struct work_struct *work)
{
	mutex_lock(&timer_keys_mutex);
	timers_update_migration();
	static_branch_enable(&timers_nohz_active);
	mutex_unlock(&timer_keys_mutex);
}