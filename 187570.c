static int sas_dev_present_in_domain(struct asd_sas_port *port,
					    u8 *sas_addr)
{
	struct domain_device *dev;

	if (SAS_ADDR(port->sas_addr) == SAS_ADDR(sas_addr))
		return 1;
	list_for_each_entry(dev, &port->dev_list, dev_list_node) {
		if (SAS_ADDR(dev->sas_addr) == SAS_ADDR(sas_addr))
			return 1;
	}
	return 0;
}