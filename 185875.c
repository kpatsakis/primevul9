static unsigned int ip6_mtu(const struct dst_entry *dst)
{
	struct inet6_dev *idev;
	unsigned int mtu = dst_metric_raw(dst, RTAX_MTU);

	if (mtu)
		goto out;

	mtu = IPV6_MIN_MTU;

	rcu_read_lock();
	idev = __in6_dev_get(dst->dev);
	if (idev)
		mtu = idev->cnf.mtu6;
	rcu_read_unlock();

out:
	return min_t(unsigned int, mtu, IP6_MAX_MTU);
}