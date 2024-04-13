static inline bool is_timers_nohz_active(void)
{
	return static_branch_unlikely(&timers_nohz_active);
}