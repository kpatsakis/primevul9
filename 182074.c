struct dentry *kvm_init_debugfs(void)
{
	d_kvm_debug = debugfs_create_dir("kvm-guest", NULL);
	if (!d_kvm_debug)
		printk(KERN_WARNING "Could not create 'kvm' debugfs directory\n");

	return d_kvm_debug;
}