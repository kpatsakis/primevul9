static void free_fdtable_rcu(struct rcu_head *rcu)
{
	__free_fdtable(container_of(rcu, struct fdtable, rcu));
}