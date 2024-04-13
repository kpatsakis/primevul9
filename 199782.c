static inline bool addrconf_qdisc_ok(const struct net_device *dev)
{
	return !qdisc_tx_is_noop(dev);
}