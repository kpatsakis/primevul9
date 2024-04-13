void addrconf_leave_solict(struct inet6_dev *idev, const struct in6_addr *addr)
{
	struct in6_addr maddr;

	if (idev->dev->flags&(IFF_LOOPBACK|IFF_NOARP))
		return;

	addrconf_addr_solict_mult(addr, &maddr);
	__ipv6_dev_mc_dec(idev, &maddr);
}