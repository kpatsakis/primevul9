nfqnl_rcv_nl_event(struct notifier_block *this,
		   unsigned long event, void *ptr)
{
	struct netlink_notify *n = ptr;
	struct nfnl_queue_net *q = nfnl_queue_pernet(n->net);

	if (event == NETLINK_URELEASE && n->protocol == NETLINK_NETFILTER) {
		int i;

		/* destroy all instances for this portid */
		spin_lock(&q->instances_lock);
		for (i = 0; i < INSTANCE_BUCKETS; i++) {
			struct hlist_node *t2;
			struct nfqnl_instance *inst;
			struct hlist_head *head = &q->instance_table[i];

			hlist_for_each_entry_safe(inst, t2, head, hlist) {
				if (n->portid == inst->peer_portid)
					__instance_destroy(inst);
			}
		}
		spin_unlock(&q->instances_lock);
	}
	return NOTIFY_DONE;
}