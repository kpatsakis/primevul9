static void os_desc_attr_release(struct config_item *item)
{
	struct os_desc *os_desc = to_os_desc(item);
	kfree(os_desc);
}