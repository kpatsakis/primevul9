static int cgw_dump_jobs(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct net *net = sock_net(skb->sk);
	struct cgw_job *gwj = NULL;
	int idx = 0;
	int s_idx = cb->args[0];

	rcu_read_lock();
	hlist_for_each_entry_rcu(gwj, &net->can.cgw_list, list) {
		if (idx < s_idx)
			goto cont;

		if (cgw_put_job(skb, gwj, RTM_NEWROUTE, NETLINK_CB(cb->skb).portid,
		    cb->nlh->nlmsg_seq, NLM_F_MULTI) < 0)
			break;
cont:
		idx++;
	}
	rcu_read_unlock();

	cb->args[0] = idx;

	return skb->len;
}