static int bpf_stats_release(struct inode *inode, struct file *file)
{
	mutex_lock(&bpf_stats_enabled_mutex);
	static_key_slow_dec(&bpf_stats_enabled_key.key);
	mutex_unlock(&bpf_stats_enabled_mutex);
	return 0;
}