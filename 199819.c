static void put_ifaddrmsg(struct nlmsghdr *nlh, u8 prefixlen, u8 flags,
			  u8 scope, int ifindex)
{
	struct ifaddrmsg *ifm;

	ifm = nlmsg_data(nlh);
	ifm->ifa_family = AF_INET6;
	ifm->ifa_prefixlen = prefixlen;
	ifm->ifa_flags = flags;
	ifm->ifa_scope = scope;
	ifm->ifa_index = ifindex;
}