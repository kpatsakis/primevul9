static ssize_t os_desc_qw_sign_store(struct config_item *item, const char *page,
				     size_t len)
{
	struct gadget_info *gi = os_desc_item_to_gadget_info(item);
	int res, l;

	l = min((int)len, OS_STRING_QW_SIGN_LEN >> 1);
	if (page[l - 1] == '\n')
		--l;

	mutex_lock(&gi->lock);
	res = utf8s_to_utf16s(page, l,
			      UTF16_LITTLE_ENDIAN, (wchar_t *) gi->qw_sign,
			      OS_STRING_QW_SIGN_LEN);
	if (res > 0)
		res = len;
	mutex_unlock(&gi->lock);

	return res;
}