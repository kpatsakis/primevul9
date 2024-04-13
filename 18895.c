static char *resolv_usage_page(unsigned page, struct seq_file *f) {
	const struct hid_usage_entry *p;
	char *buf = NULL;

	if (!f) {
		buf = kzalloc(sizeof(char) * HID_DEBUG_BUFSIZE, GFP_ATOMIC);
		if (!buf)
			return ERR_PTR(-ENOMEM);
	}

	for (p = hid_usage_table; p->description; p++)
		if (p->page == page) {
			if (!f) {
				snprintf(buf, HID_DEBUG_BUFSIZE, "%s",
						p->description);
				return buf;
			}
			else {
				seq_printf(f, "%s", p->description);
				return NULL;
			}
		}
	if (!f)
		snprintf(buf, HID_DEBUG_BUFSIZE, "%04x", page);
	else
		seq_printf(f, "%04x", page);
	return buf;
}