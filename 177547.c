isdn_net_force_dial(char *name)
{
	isdn_net_dev *p = isdn_net_findif(name);

	if (!p)
		return -ENODEV;
	return (isdn_net_force_dial_lp(p->local));
}