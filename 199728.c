static int inet6_fill_ifaddr(struct sk_buff *skb, struct inet6_ifaddr *ifa,
			     u32 portid, u32 seq, int event, unsigned int flags)
{
	struct nlmsghdr  *nlh;
	u32 preferred, valid;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(struct ifaddrmsg), flags);
	if (nlh == NULL)
		return -EMSGSIZE;

	put_ifaddrmsg(nlh, ifa->prefix_len, ifa->flags, rt_scope(ifa->scope),
		      ifa->idev->dev->ifindex);

	if (!(ifa->flags&IFA_F_PERMANENT)) {
		preferred = ifa->prefered_lft;
		valid = ifa->valid_lft;
		if (preferred != INFINITY_LIFE_TIME) {
			long tval = (jiffies - ifa->tstamp)/HZ;
			if (preferred > tval)
				preferred -= tval;
			else
				preferred = 0;
			if (valid != INFINITY_LIFE_TIME) {
				if (valid > tval)
					valid -= tval;
				else
					valid = 0;
			}
		}
	} else {
		preferred = INFINITY_LIFE_TIME;
		valid = INFINITY_LIFE_TIME;
	}

	if (!ipv6_addr_any(&ifa->peer_addr)) {
		if (nla_put(skb, IFA_LOCAL, 16, &ifa->addr) < 0 ||
		    nla_put(skb, IFA_ADDRESS, 16, &ifa->peer_addr) < 0)
			goto error;
	} else
		if (nla_put(skb, IFA_ADDRESS, 16, &ifa->addr) < 0)
			goto error;

	if (put_cacheinfo(skb, ifa->cstamp, ifa->tstamp, preferred, valid) < 0)
		goto error;

	return nlmsg_end(skb, nlh);

error:
	nlmsg_cancel(skb, nlh);
	return -EMSGSIZE;
}