static ssize_t os_desc_b_vendor_code_store(struct config_item *item,
					   const char *page, size_t len)
{
	struct gadget_info *gi = os_desc_item_to_gadget_info(item);
	int ret;
	u8 b_vendor_code;

	mutex_lock(&gi->lock);
	ret = kstrtou8(page, 0, &b_vendor_code);
	if (!ret) {
		gi->b_vendor_code = b_vendor_code;
		ret = len;
	}
	mutex_unlock(&gi->lock);

	return ret;
}