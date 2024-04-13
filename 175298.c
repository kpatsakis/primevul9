static int get_dpifindex(struct datapath *dp)
{
	struct vport *local;
	int ifindex;

	rcu_read_lock();

	local = ovs_vport_rcu(dp, OVSP_LOCAL);
	if (local)
		ifindex = netdev_vport_priv(local)->dev->ifindex;
	else
		ifindex = 0;

	rcu_read_unlock();

	return ifindex;
}