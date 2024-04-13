static struct slave *bond_xmit_activebackup_slave_get(struct bonding *bond,
						      struct sk_buff *skb)
{
	return rcu_dereference(bond->curr_active_slave);
}