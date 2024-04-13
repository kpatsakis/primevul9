int ipv6_chk_prefix(const struct in6_addr *addr, struct net_device *dev)
{
	struct inet6_dev *idev;
	struct inet6_ifaddr *ifa;
	int	onlink;

	onlink = 0;
	rcu_read_lock();
	idev = __in6_dev_get(dev);
	if (idev) {
		read_lock_bh(&idev->lock);
		list_for_each_entry(ifa, &idev->addr_list, if_list) {
			onlink = ipv6_prefix_equal(addr, &ifa->addr,
						   ifa->prefix_len);
			if (onlink)
				break;
		}
		read_unlock_bh(&idev->lock);
	}
	rcu_read_unlock();
	return onlink;
}