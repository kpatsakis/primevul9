static int kvm_nx_lpage_recovery_worker(struct kvm *kvm, uintptr_t data)
{
	u64 start_time;
	long remaining_time;

	while (true) {
		start_time = get_jiffies_64();
		remaining_time = get_nx_lpage_recovery_timeout(start_time);

		set_current_state(TASK_INTERRUPTIBLE);
		while (!kthread_should_stop() && remaining_time > 0) {
			schedule_timeout(remaining_time);
			remaining_time = get_nx_lpage_recovery_timeout(start_time);
			set_current_state(TASK_INTERRUPTIBLE);
		}

		set_current_state(TASK_RUNNING);

		if (kthread_should_stop())
			return 0;

		kvm_recover_nx_lpages(kvm);
	}
}