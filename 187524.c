static void sas_unregister_ex_tree(struct asd_sas_port *port, struct domain_device *dev)
{
	struct expander_device *ex = &dev->ex_dev;
	struct domain_device *child, *n;

	list_for_each_entry_safe(child, n, &ex->children, siblings) {
		set_bit(SAS_DEV_GONE, &child->state);
		if (child->dev_type == SAS_EDGE_EXPANDER_DEVICE ||
		    child->dev_type == SAS_FANOUT_EXPANDER_DEVICE)
			sas_unregister_ex_tree(port, child);
		else
			sas_unregister_dev(port, child);
	}
	sas_unregister_dev(port, dev);
}