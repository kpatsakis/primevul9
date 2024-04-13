static int __ip6_ins_rt(struct rt6_info *rt, struct nl_info *info,
			struct nlattr *mx, int mx_len)
{
	int err;
	struct fib6_table *table;

	table = rt->rt6i_table;
	write_lock_bh(&table->tb6_lock);
	err = fib6_add(&table->tb6_root, rt, info, mx, mx_len);
	write_unlock_bh(&table->tb6_lock);

	return err;
}