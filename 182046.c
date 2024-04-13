static void apf_task_wake_all(void)
{
	int i;

	for (i = 0; i < KVM_TASK_SLEEP_HASHSIZE; i++) {
		struct hlist_node *p, *next;
		struct kvm_task_sleep_head *b = &async_pf_sleepers[i];
		spin_lock(&b->lock);
		hlist_for_each_safe(p, next, &b->list) {
			struct kvm_task_sleep_node *n =
				hlist_entry(p, typeof(*n), link);
			if (n->cpu == smp_processor_id())
				apf_task_wake_one(n);
		}
		spin_unlock(&b->lock);
	}
}