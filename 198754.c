static ssize_t os_desc_use_store(struct config_item *item, const char *page,
				 size_t len)
{
	struct gadget_info *gi = os_desc_item_to_gadget_info(item);
	int ret;
	bool use;

	mutex_lock(&gi->lock);
	ret = strtobool(page, &use);
	if (!ret) {
		gi->use_os_desc = use;
		ret = len;
	}
	mutex_unlock(&gi->lock);

	return ret;
}