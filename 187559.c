static char sas_route_char(struct domain_device *dev, struct ex_phy *phy)
{
	switch (phy->routing_attr) {
	case TABLE_ROUTING:
		if (dev->ex_dev.t2t_supp)
			return 'U';
		else
			return 'T';
	case DIRECT_ROUTING:
		return 'D';
	case SUBTRACTIVE_ROUTING:
		return 'S';
	default:
		return '?';
	}
}