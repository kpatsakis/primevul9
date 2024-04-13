static int set_nx_huge_pages_recovery_param(const char *val, const struct kernel_param *kp)
{
	bool was_recovery_enabled, is_recovery_enabled;
	uint old_period, new_period;
	int err;

	was_recovery_enabled = calc_nx_huge_pages_recovery_period(&old_period);

	err = param_set_uint(val, kp);
	if (err)
		return err;

	is_recovery_enabled = calc_nx_huge_pages_recovery_period(&new_period);

	if (is_recovery_enabled &&
	    (!was_recovery_enabled || old_period > new_period)) {
		struct kvm *kvm;

		mutex_lock(&kvm_lock);

		list_for_each_entry(kvm, &vm_list, vm_list)
			wake_up_process(kvm->arch.nx_lpage_recovery_thread);

		mutex_unlock(&kvm_lock);
	}

	return err;
}