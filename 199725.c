static int inet6_fill_ifinfo(struct sk_buff *skb, struct inet6_dev *idev,
			     u32 portid, u32 seq, int event, unsigned int flags)
{
	struct net_device *dev = idev->dev;
	struct ifinfomsg *hdr;
	struct nlmsghdr *nlh;
	void *protoinfo;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(*hdr), flags);
	if (nlh == NULL)
		return -EMSGSIZE;

	hdr = nlmsg_data(nlh);
	hdr->ifi_family = AF_INET6;
	hdr->__ifi_pad = 0;
	hdr->ifi_type = dev->type;
	hdr->ifi_index = dev->ifindex;
	hdr->ifi_flags = dev_get_flags(dev);
	hdr->ifi_change = 0;

	if (nla_put_string(skb, IFLA_IFNAME, dev->name) ||
	    (dev->addr_len &&
	     nla_put(skb, IFLA_ADDRESS, dev->addr_len, dev->dev_addr)) ||
	    nla_put_u32(skb, IFLA_MTU, dev->mtu) ||
	    (dev->ifindex != dev->iflink &&
	     nla_put_u32(skb, IFLA_LINK, dev->iflink)))
		goto nla_put_failure;
	protoinfo = nla_nest_start(skb, IFLA_PROTINFO);
	if (protoinfo == NULL)
		goto nla_put_failure;

	if (inet6_fill_ifla6_attrs(skb, idev) < 0)
		goto nla_put_failure;

	nla_nest_end(skb, protoinfo);
	return nlmsg_end(skb, nlh);

nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}