static struct net_device *__bond_sk_get_lower_dev(struct bonding *bond,
						  struct sock *sk)
{
	struct bond_up_slave *slaves;
	struct slave *slave;
	unsigned int count;
	u32 hash;

	slaves = rcu_dereference(bond->usable_slaves);
	count = slaves ? READ_ONCE(slaves->count) : 0;
	if (unlikely(!count))
		return NULL;

	hash = bond_sk_hash_l34(sk);
	slave = slaves->arr[hash % count];

	return slave->dev;
}