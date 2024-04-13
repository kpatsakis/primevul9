static size_t inet6_get_link_af_size(const struct net_device *dev)
{
	if (!__in6_dev_get(dev))
		return 0;

	return inet6_ifla6_size();
}