void netlink_ack(struct sk_buff *in_skb, struct nlmsghdr *nlh, int err,
		 const struct netlink_ext_ack *extack)
{
	struct sk_buff *skb;
	struct nlmsghdr *rep;
	struct nlmsgerr *errmsg;
	size_t payload = sizeof(*errmsg);
	size_t tlvlen = 0;
	struct netlink_sock *nlk = nlk_sk(NETLINK_CB(in_skb).sk);
	unsigned int flags = 0;
	bool nlk_has_extack = nlk->flags & NETLINK_F_EXT_ACK;

	/* Error messages get the original request appened, unless the user
	 * requests to cap the error message, and get extra error data if
	 * requested.
	 */
	if (nlk_has_extack && extack && extack->_msg)
		tlvlen += nla_total_size(strlen(extack->_msg) + 1);

	if (err) {
		if (!(nlk->flags & NETLINK_F_CAP_ACK))
			payload += nlmsg_len(nlh);
		else
			flags |= NLM_F_CAPPED;
		if (nlk_has_extack && extack && extack->bad_attr)
			tlvlen += nla_total_size(sizeof(u32));
	} else {
		flags |= NLM_F_CAPPED;

		if (nlk_has_extack && extack && extack->cookie_len)
			tlvlen += nla_total_size(extack->cookie_len);
	}

	if (tlvlen)
		flags |= NLM_F_ACK_TLVS;

	skb = nlmsg_new(payload + tlvlen, GFP_KERNEL);
	if (!skb) {
		NETLINK_CB(in_skb).sk->sk_err = ENOBUFS;
		NETLINK_CB(in_skb).sk->sk_error_report(NETLINK_CB(in_skb).sk);
		return;
	}

	rep = __nlmsg_put(skb, NETLINK_CB(in_skb).portid, nlh->nlmsg_seq,
			  NLMSG_ERROR, payload, flags);
	errmsg = nlmsg_data(rep);
	errmsg->error = err;
	memcpy(&errmsg->msg, nlh, payload > sizeof(*errmsg) ? nlh->nlmsg_len : sizeof(*nlh));

	if (nlk_has_extack && extack) {
		if (extack->_msg) {
			WARN_ON(nla_put_string(skb, NLMSGERR_ATTR_MSG,
					       extack->_msg));
		}
		if (err) {
			if (extack->bad_attr &&
			    !WARN_ON((u8 *)extack->bad_attr < in_skb->data ||
				     (u8 *)extack->bad_attr >= in_skb->data +
							       in_skb->len))
				WARN_ON(nla_put_u32(skb, NLMSGERR_ATTR_OFFS,
						    (u8 *)extack->bad_attr -
						    in_skb->data));
		} else {
			if (extack->cookie_len)
				WARN_ON(nla_put(skb, NLMSGERR_ATTR_COOKIE,
						extack->cookie_len,
						extack->cookie));
		}
	}

	nlmsg_end(skb, rep);

	netlink_unicast(in_skb->sk, skb, NETLINK_CB(in_skb).portid, MSG_DONTWAIT);
}