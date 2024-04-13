static int __init kvm_spinlock_debugfs(void)
{
	struct dentry *d_kvm;

	d_kvm = kvm_init_debugfs();
	if (d_kvm == NULL)
		return -ENOMEM;

	d_spin_debug = debugfs_create_dir("spinlocks", d_kvm);

	debugfs_create_u8("zero_stats", 0644, d_spin_debug, &zero_stats);

	debugfs_create_u32("taken_slow", 0444, d_spin_debug,
		   &spinlock_stats.contention_stats[TAKEN_SLOW]);
	debugfs_create_u32("taken_slow_pickup", 0444, d_spin_debug,
		   &spinlock_stats.contention_stats[TAKEN_SLOW_PICKUP]);

	debugfs_create_u32("released_slow", 0444, d_spin_debug,
		   &spinlock_stats.contention_stats[RELEASED_SLOW]);
	debugfs_create_u32("released_slow_kicked", 0444, d_spin_debug,
		   &spinlock_stats.contention_stats[RELEASED_SLOW_KICKED]);

	debugfs_create_u64("time_blocked", 0444, d_spin_debug,
			   &spinlock_stats.time_blocked);

	debugfs_create_u32_array("histo_blocked", 0444, d_spin_debug,
		     spinlock_stats.histo_spin_blocked, HISTO_BUCKETS + 1);

	return 0;
}