bool bond_sk_check(struct bonding *bond)
{
	switch (BOND_MODE(bond)) {
	case BOND_MODE_8023AD:
	case BOND_MODE_XOR:
		if (bond->params.xmit_policy == BOND_XMIT_POLICY_LAYER34)
			return true;
		fallthrough;
	default:
		return false;
	}
}