void kvm_async_pf_task_wake(u32 token)
{
	u32 key = hash_32(token, KVM_TASK_SLEEP_HASHBITS);
	struct kvm_task_sleep_head *b = &async_pf_sleepers[key];
	struct kvm_task_sleep_node *n;

	if (token == ~0) {
		apf_task_wake_all();
		return;
	}

again:
	spin_lock(&b->lock);
	n = _find_apf_task(b, token);
	if (!n) {
		/*
		 * async PF was not yet handled.
		 * Add dummy entry for the token.
		 */
		n = kzalloc(sizeof(*n), GFP_ATOMIC);
		if (!n) {
			/*
			 * Allocation failed! Busy wait while other cpu
			 * handles async PF.
			 */
			spin_unlock(&b->lock);
			cpu_relax();
			goto again;
		}
		n->token = token;
		n->cpu = smp_processor_id();
		init_waitqueue_head(&n->wq);
		hlist_add_head(&n->link, &b->list);
	} else
		apf_task_wake_one(n);
	spin_unlock(&b->lock);
	return;
}