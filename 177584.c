isdn_net_findif(char *name)
{
	isdn_net_dev *p = dev->netdev;

	while (p) {
		if (!strcmp(p->dev->name, name))
			return p;
		p = (isdn_net_dev *) p->next;
	}
	return (isdn_net_dev *) NULL;
}