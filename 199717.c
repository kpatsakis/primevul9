ipv6_link_dev_addr(struct inet6_dev *idev, struct inet6_ifaddr *ifp)
{
	struct list_head *p;
	int ifp_scope = ipv6_addr_src_scope(&ifp->addr);

	/*
	 * Each device address list is sorted in order of scope -
	 * global before linklocal.
	 */
	list_for_each(p, &idev->addr_list) {
		struct inet6_ifaddr *ifa
			= list_entry(p, struct inet6_ifaddr, if_list);
		if (ifp_scope >= ipv6_addr_src_scope(&ifa->addr))
			break;
	}

	list_add_tail(&ifp->if_list, p);
}