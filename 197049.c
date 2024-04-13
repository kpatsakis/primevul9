static void bond_miimon_link_change(struct bonding *bond,
				    struct slave *slave,
				    char link)
{
	switch (BOND_MODE(bond)) {
	case BOND_MODE_8023AD:
		bond_3ad_handle_link_change(slave, link);
		break;
	case BOND_MODE_TLB:
	case BOND_MODE_ALB:
		bond_alb_handle_link_change(bond, slave, link);
		break;
	case BOND_MODE_XOR:
		bond_update_slave_arr(bond, NULL);
		break;
	}
}