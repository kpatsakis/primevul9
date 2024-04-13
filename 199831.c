int __ipv6_get_lladdr(struct inet6_dev *idev, struct in6_addr *addr,
		      unsigned char banned_flags)
{
	struct inet6_ifaddr *ifp;
	int err = -EADDRNOTAVAIL;

	list_for_each_entry(ifp, &idev->addr_list, if_list) {
		if (ifp->scope == IFA_LINK &&
		    !(ifp->flags & banned_flags)) {
			*addr = ifp->addr;
			err = 0;
			break;
		}
	}
	return err;
}