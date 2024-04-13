static struct net_device *bond_xmit_get_slave(struct net_device *master_dev,
					      struct sk_buff *skb,
					      bool all_slaves)
{
	struct bonding *bond = netdev_priv(master_dev);
	struct bond_up_slave *slaves;
	struct slave *slave = NULL;

	switch (BOND_MODE(bond)) {
	case BOND_MODE_ROUNDROBIN:
		slave = bond_xmit_roundrobin_slave_get(bond, skb);
		break;
	case BOND_MODE_ACTIVEBACKUP:
		slave = bond_xmit_activebackup_slave_get(bond, skb);
		break;
	case BOND_MODE_8023AD:
	case BOND_MODE_XOR:
		if (all_slaves)
			slaves = rcu_dereference(bond->all_slaves);
		else
			slaves = rcu_dereference(bond->usable_slaves);
		slave = bond_xmit_3ad_xor_slave_get(bond, skb, slaves);
		break;
	case BOND_MODE_BROADCAST:
		break;
	case BOND_MODE_ALB:
		slave = bond_xmit_alb_slave_get(bond, skb);
		break;
	case BOND_MODE_TLB:
		slave = bond_xmit_tlb_slave_get(bond, skb);
		break;
	default:
		/* Should never happen, mode already checked */
		WARN_ONCE(true, "Unknown bonding mode");
		break;
	}

	if (slave)
		return slave->dev;
	return NULL;
}