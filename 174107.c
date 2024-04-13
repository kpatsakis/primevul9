
static void kvm_destroy_vm_debugfs(struct kvm *kvm)
{
	int i;
	int kvm_debugfs_num_entries = kvm_vm_stats_header.num_desc +
				      kvm_vcpu_stats_header.num_desc;

	if (!kvm->debugfs_dentry)
		return;

	debugfs_remove_recursive(kvm->debugfs_dentry);

	if (kvm->debugfs_stat_data) {
		for (i = 0; i < kvm_debugfs_num_entries; i++)
			kfree(kvm->debugfs_stat_data[i]);
		kfree(kvm->debugfs_stat_data);
	}