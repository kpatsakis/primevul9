static void gadget_config_attr_release(struct config_item *item)
{
	struct config_usb_cfg *cfg = to_config_usb_cfg(item);

	WARN_ON(!list_empty(&cfg->c.functions));
	list_del(&cfg->c.list);
	kfree(cfg->c.label);
	kfree(cfg);
}