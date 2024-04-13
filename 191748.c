int netlink_rcv_skb(struct sk_buff *skb, int (*cb)(struct sk_buff *,
						   struct nlmsghdr *,
						   struct netlink_ext_ack *))
{
	struct netlink_ext_ack extack = {};
	struct nlmsghdr *nlh;
	int err;

	while (skb->len >= nlmsg_total_size(0)) {
		int msglen;

		nlh = nlmsg_hdr(skb);
		err = 0;

		if (nlh->nlmsg_len < NLMSG_HDRLEN || skb->len < nlh->nlmsg_len)
			return 0;

		/* Only requests are handled by the kernel */
		if (!(nlh->nlmsg_flags & NLM_F_REQUEST))
			goto ack;

		/* Skip control messages */
		if (nlh->nlmsg_type < NLMSG_MIN_TYPE)
			goto ack;

		err = cb(skb, nlh, &extack);
		if (err == -EINTR)
			goto skip;

ack:
		if (nlh->nlmsg_flags & NLM_F_ACK || err)
			netlink_ack(skb, nlh, err, &extack);

skip:
		msglen = NLMSG_ALIGN(nlh->nlmsg_len);
		if (msglen > skb->len)
			msglen = skb->len;
		skb_pull(skb, msglen);
	}

	return 0;
}