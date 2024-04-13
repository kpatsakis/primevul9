nfqnl_recv_verdict(struct sock *ctnl, struct sk_buff *skb,
		   const struct nlmsghdr *nlh,
		   const struct nlattr * const nfqa[])
{
	struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	u_int16_t queue_num = ntohs(nfmsg->res_id);

	struct nfqnl_msg_verdict_hdr *vhdr;
	struct nfqnl_instance *queue;
	unsigned int verdict;
	struct nf_queue_entry *entry;
	enum ip_conntrack_info uninitialized_var(ctinfo);
	struct nf_conn *ct = NULL;

	struct net *net = sock_net(ctnl);
	struct nfnl_queue_net *q = nfnl_queue_pernet(net);

	queue = instance_lookup(q, queue_num);
	if (!queue)
		queue = verdict_instance_lookup(q, queue_num,
						NETLINK_CB(skb).portid);
	if (IS_ERR(queue))
		return PTR_ERR(queue);

	vhdr = verdicthdr_get(nfqa);
	if (!vhdr)
		return -EINVAL;

	verdict = ntohl(vhdr->verdict);

	entry = find_dequeue_entry(queue, ntohl(vhdr->id));
	if (entry == NULL)
		return -ENOENT;

	if (nfqa[NFQA_CT]) {
		ct = nfqnl_ct_parse(entry->skb, nfqa[NFQA_CT], &ctinfo);
		if (ct && nfqa[NFQA_EXP]) {
			nfqnl_attach_expect(ct, nfqa[NFQA_EXP],
					    NETLINK_CB(skb).portid,
					    nlmsg_report(nlh));
		}
	}

	if (nfqa[NFQA_PAYLOAD]) {
		u16 payload_len = nla_len(nfqa[NFQA_PAYLOAD]);
		int diff = payload_len - entry->skb->len;

		if (nfqnl_mangle(nla_data(nfqa[NFQA_PAYLOAD]),
				 payload_len, entry, diff) < 0)
			verdict = NF_DROP;

		if (ct)
			nfqnl_ct_seq_adjust(entry->skb, ct, ctinfo, diff);
	}

	if (nfqa[NFQA_MARK])
		entry->skb->mark = ntohl(nla_get_be32(nfqa[NFQA_MARK]));

	nf_reinject(entry, verdict);
	return 0;
}