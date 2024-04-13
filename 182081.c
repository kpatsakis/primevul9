void kvm_async_pf_task_wait(u32 token)
{
	u32 key = hash_32(token, KVM_TASK_SLEEP_HASHBITS);
	struct kvm_task_sleep_head *b = &async_pf_sleepers[key];
	struct kvm_task_sleep_node n, *e;
	DEFINE_WAIT(wait);

	rcu_irq_enter();

	spin_lock(&b->lock);
	e = _find_apf_task(b, token);
	if (e) {
		/* dummy entry exist -> wake up was delivered ahead of PF */
		hlist_del(&e->link);
		kfree(e);
		spin_unlock(&b->lock);

		rcu_irq_exit();
		return;
	}

	n.token = token;
	n.cpu = smp_processor_id();
	n.halted = is_idle_task(current) || preempt_count() > 1;
	init_waitqueue_head(&n.wq);
	hlist_add_head(&n.link, &b->list);
	spin_unlock(&b->lock);

	for (;;) {
		if (!n.halted)
			prepare_to_wait(&n.wq, &wait, TASK_UNINTERRUPTIBLE);
		if (hlist_unhashed(&n.link))
			break;

		if (!n.halted) {
			local_irq_enable();
			schedule();
			local_irq_disable();
		} else {
			/*
			 * We cannot reschedule. So halt.
			 */
			rcu_irq_exit();
			native_safe_halt();
			rcu_irq_enter();
			local_irq_disable();
		}
	}
	if (!n.halted)
		finish_wait(&n.wq, &wait);

	rcu_irq_exit();
	return;
}