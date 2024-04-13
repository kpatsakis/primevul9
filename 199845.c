int ipv6_get_lladdr(struct net_device *dev, struct in6_addr *addr,
		    unsigned char banned_flags)
{
	struct inet6_dev *idev;
	int err = -EADDRNOTAVAIL;

	rcu_read_lock();
	idev = __in6_dev_get(dev);
	if (idev) {
		read_lock_bh(&idev->lock);
		err = __ipv6_get_lladdr(idev, addr, banned_flags);
		read_unlock_bh(&idev->lock);
	}
	rcu_read_unlock();
	return err;
}