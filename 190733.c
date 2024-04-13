static inline bool f2fs_time_over(struct f2fs_sb_info *sbi, int type)
{
	unsigned long interval = sbi->interval_time[type] * HZ;

	return time_after(jiffies, sbi->last_time[type] + interval);
}