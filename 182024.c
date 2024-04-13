static void apf_task_wake_one(struct kvm_task_sleep_node *n)
{
	hlist_del_init(&n->link);
	if (n->halted)
		smp_send_reschedule(n->cpu);
	else if (waitqueue_active(&n->wq))
		wake_up(&n->wq);
}