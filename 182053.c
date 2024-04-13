static struct kvm_task_sleep_node *_find_apf_task(struct kvm_task_sleep_head *b,
						  u32 token)
{
	struct hlist_node *p;

	hlist_for_each(p, &b->list) {
		struct kvm_task_sleep_node *n =
			hlist_entry(p, typeof(*n), link);
		if (n->token == token)
			return n;
	}

	return NULL;
}