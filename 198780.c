static void gadget_config_name_attr_release(struct config_item *item)
{
	struct gadget_config_name *cn = to_gadget_config_name(item);

	kfree(cn->configuration);

	list_del(&cn->list);
	kfree(cn);
}