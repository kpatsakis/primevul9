static struct net_device *bond_sk_get_lower_dev(struct net_device *dev,
						struct sock *sk)
{
	struct bonding *bond = netdev_priv(dev);
	struct net_device *lower = NULL;

	rcu_read_lock();
	if (bond_sk_check(bond))
		lower = __bond_sk_get_lower_dev(bond, sk);
	rcu_read_unlock();

	return lower;
}