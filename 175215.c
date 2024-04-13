static void destroy_dp_rcu(struct rcu_head *rcu)
{
	struct datapath *dp = container_of(rcu, struct datapath, rcu);

	free_percpu(dp->stats_percpu);
	release_net(ovs_dp_get_net(dp));
	kfree(dp->ports);
	kfree(dp);
}