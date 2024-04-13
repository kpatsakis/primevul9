static int inet6_fill_prefix(struct sk_buff *skb, struct inet6_dev *idev,
			     struct prefix_info *pinfo, u32 portid, u32 seq,
			     int event, unsigned int flags)
{
	struct prefixmsg *pmsg;
	struct nlmsghdr *nlh;
	struct prefix_cacheinfo	ci;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(*pmsg), flags);
	if (nlh == NULL)
		return -EMSGSIZE;

	pmsg = nlmsg_data(nlh);
	pmsg->prefix_family = AF_INET6;
	pmsg->prefix_pad1 = 0;
	pmsg->prefix_pad2 = 0;
	pmsg->prefix_ifindex = idev->dev->ifindex;
	pmsg->prefix_len = pinfo->prefix_len;
	pmsg->prefix_type = pinfo->type;
	pmsg->prefix_pad3 = 0;
	pmsg->prefix_flags = 0;
	if (pinfo->onlink)
		pmsg->prefix_flags |= IF_PREFIX_ONLINK;
	if (pinfo->autoconf)
		pmsg->prefix_flags |= IF_PREFIX_AUTOCONF;

	if (nla_put(skb, PREFIX_ADDRESS, sizeof(pinfo->prefix), &pinfo->prefix))
		goto nla_put_failure;
	ci.preferred_time = ntohl(pinfo->prefered);
	ci.valid_time = ntohl(pinfo->valid);
	if (nla_put(skb, PREFIX_CACHEINFO, sizeof(ci), &ci))
		goto nla_put_failure;
	return nlmsg_end(skb, nlh);

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}