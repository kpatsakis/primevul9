ipv6_inherit_linklocal(struct inet6_dev *idev, struct net_device *link_dev)
{
	struct in6_addr lladdr;

	if (!ipv6_get_lladdr(link_dev, &lladdr, IFA_F_TENTATIVE)) {
		addrconf_add_linklocal(idev, &lladdr);
		return 0;
	}
	return -1;
}