nfqnl_recv_verdict_batch(struct sock *ctnl, struct sk_buff *skb,
		   const struct nlmsghdr *nlh,
		   const struct nlattr * const nfqa[])
{
	struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	struct nf_queue_entry *entry, *tmp;
	unsigned int verdict, maxid;
	struct nfqnl_msg_verdict_hdr *vhdr;
	struct nfqnl_instance *queue;
	LIST_HEAD(batch_list);
	u16 queue_num = ntohs(nfmsg->res_id);

	struct net *net = sock_net(ctnl);
	struct nfnl_queue_net *q = nfnl_queue_pernet(net);

	queue = verdict_instance_lookup(q, queue_num,
					NETLINK_CB(skb).portid);
	if (IS_ERR(queue))
		return PTR_ERR(queue);

	vhdr = verdicthdr_get(nfqa);
	if (!vhdr)
		return -EINVAL;

	verdict = ntohl(vhdr->verdict);
	maxid = ntohl(vhdr->id);

	spin_lock_bh(&queue->lock);

	list_for_each_entry_safe(entry, tmp, &queue->queue_list, list) {
		if (nfq_id_after(entry->id, maxid))
			break;
		__dequeue_entry(queue, entry);
		list_add_tail(&entry->list, &batch_list);
	}

	spin_unlock_bh(&queue->lock);

	if (list_empty(&batch_list))
		return -ENOENT;

	list_for_each_entry_safe(entry, tmp, &batch_list, list) {
		if (nfqa[NFQA_MARK])
			entry->skb->mark = ntohl(nla_get_be32(nfqa[NFQA_MARK]));
		nf_reinject(entry, verdict);
	}
	return 0;
}