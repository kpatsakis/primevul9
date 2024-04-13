static ssize_t os_desc_b_vendor_code_show(struct config_item *item, char *page)
{
	return sprintf(page, "0x%02x\n",
			os_desc_item_to_gadget_info(item)->b_vendor_code);
}