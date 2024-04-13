static void timers_update_migration(void)
{
	if (sysctl_timer_migration && tick_nohz_active)
		static_branch_enable(&timers_migration_enabled);
	else
		static_branch_disable(&timers_migration_enabled);
}