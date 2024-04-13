const char *bond_slave_link_status(s8 link)
{
	switch (link) {
	case BOND_LINK_UP:
		return "up";
	case BOND_LINK_FAIL:
		return "going down";
	case BOND_LINK_DOWN:
		return "down";
	case BOND_LINK_BACK:
		return "going back";
	default:
		return "unknown";
	}
}