static int sas_ex_level_discovery(struct asd_sas_port *port, const int level)
{
	int res = 0;
	struct domain_device *dev;

	list_for_each_entry(dev, &port->dev_list, dev_list_node) {
		if (dev->dev_type == SAS_EDGE_EXPANDER_DEVICE ||
		    dev->dev_type == SAS_FANOUT_EXPANDER_DEVICE) {
			struct sas_expander_device *ex =
				rphy_to_expander_device(dev->rphy);

			if (level == ex->level)
				res = sas_ex_discover_devices(dev, -1);
			else if (level > 0)
				res = sas_ex_discover_devices(port->port_dev, -1);

		}
	}

	return res;
}