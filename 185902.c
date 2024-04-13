static int __ip6_del_rt(struct rt6_info *rt, struct nl_info *info)
{
	int err;
	struct fib6_table *table;
	struct net *net = dev_net(rt->dst.dev);

	if (rt == net->ipv6.ip6_null_entry) {
		err = -ENOENT;
		goto out;
	}

	table = rt->rt6i_table;
	write_lock_bh(&table->tb6_lock);
	err = fib6_del(rt, info);
	write_unlock_bh(&table->tb6_lock);

out:
	ip6_rt_put(rt);
	return err;
}