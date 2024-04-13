int check_user_usb_string(const char *name,
		struct usb_gadget_strings *stringtab_dev)
{
	unsigned primary_lang;
	unsigned sub_lang;
	u16 num;
	int ret;

	ret = kstrtou16(name, 0, &num);
	if (ret)
		return ret;

	primary_lang = num & 0x3ff;
	sub_lang = num >> 10;

	/* simple sanity check for valid langid */
	switch (primary_lang) {
	case 0:
	case 0x62 ... 0xfe:
	case 0x100 ... 0x3ff:
		return -EINVAL;
	}
	if (!sub_lang)
		return -EINVAL;

	stringtab_dev->language = num;
	return 0;
}