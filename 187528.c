static int sas_disable_routing(struct domain_device *dev,  u8 *sas_addr)
{
	if (dev->parent)
		return sas_configure_parent(dev->parent, dev, sas_addr, 0);
	return 0;
}