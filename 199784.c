static void  __ipv6_try_regen_rndid(struct inet6_dev *idev, struct in6_addr *tmpaddr)
{
	if (tmpaddr && memcmp(idev->rndid, &tmpaddr->s6_addr[8], 8) == 0)
		__ipv6_regen_rndid(idev);
}