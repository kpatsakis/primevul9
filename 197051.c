static int bond_update_speed_duplex(struct slave *slave)
{
	struct net_device *slave_dev = slave->dev;
	struct ethtool_link_ksettings ecmd;
	int res;

	slave->speed = SPEED_UNKNOWN;
	slave->duplex = DUPLEX_UNKNOWN;

	res = __ethtool_get_link_ksettings(slave_dev, &ecmd);
	if (res < 0)
		return 1;
	if (ecmd.base.speed == 0 || ecmd.base.speed == ((__u32)-1))
		return 1;
	switch (ecmd.base.duplex) {
	case DUPLEX_FULL:
	case DUPLEX_HALF:
		break;
	default:
		return 1;
	}

	slave->speed = ecmd.base.speed;
	slave->duplex = ecmd.base.duplex;

	return 0;
}