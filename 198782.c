static ssize_t os_desc_qw_sign_show(struct config_item *item, char *page)
{
	struct gadget_info *gi = os_desc_item_to_gadget_info(item);
	int res;

	res = utf16s_to_utf8s((wchar_t *) gi->qw_sign, OS_STRING_QW_SIGN_LEN,
			      UTF16_LITTLE_ENDIAN, page, PAGE_SIZE - 1);
	page[res++] = '\n';

	return res;
}