static int set_nx_huge_pages(const char *val, const struct kernel_param *kp)
{
	bool old_val = nx_huge_pages;
	bool new_val;

	/* In "auto" mode deploy workaround only if CPU has the bug. */
	if (sysfs_streq(val, "off"))
		new_val = 0;
	else if (sysfs_streq(val, "force"))
		new_val = 1;
	else if (sysfs_streq(val, "auto"))
		new_val = get_nx_auto_mode();
	else if (strtobool(val, &new_val) < 0)
		return -EINVAL;

	__set_nx_huge_pages(new_val);

	if (new_val != old_val) {
		struct kvm *kvm;

		mutex_lock(&kvm_lock);

		list_for_each_entry(kvm, &vm_list, vm_list) {
			mutex_lock(&kvm->slots_lock);
			kvm_mmu_zap_all_fast(kvm);
			mutex_unlock(&kvm->slots_lock);

			wake_up_process(kvm->arch.nx_lpage_recovery_thread);
		}
		mutex_unlock(&kvm_lock);
	}

	return 0;
}