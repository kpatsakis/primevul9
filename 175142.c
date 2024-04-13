static struct datapath *get_dp(struct net *net, int dp_ifindex)
{
	struct datapath *dp = NULL;
	struct net_device *dev;

	rcu_read_lock();
	dev = dev_get_by_index_rcu(net, dp_ifindex);
	if (dev) {
		struct vport *vport = ovs_internal_dev_get_vport(dev);
		if (vport)
			dp = vport->dp;
	}
	rcu_read_unlock();

	return dp;
}