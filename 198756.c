static ssize_t os_desc_use_show(struct config_item *item, char *page)
{
	return sprintf(page, "%d\n",
			os_desc_item_to_gadget_info(item)->use_os_desc);
}