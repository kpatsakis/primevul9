int timer_migration_handler(struct ctl_table *table, int write,
			    void *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;

	mutex_lock(&timer_keys_mutex);
	ret = proc_dointvec_minmax(table, write, buffer, lenp, ppos);
	if (!ret && write)
		timers_update_migration();
	mutex_unlock(&timer_keys_mutex);
	return ret;
}