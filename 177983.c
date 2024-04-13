static char *phar_get_link_location(phar_entry_info *entry) /* {{{ */
{
	char *p, *ret = NULL;
	if (!entry->link) {
		return NULL;
	}
	if (entry->link[0] == '/') {
		return estrdup(entry->link + 1);
	}
	p = strrchr(entry->filename, '/');
	if (p) {
		*p = '\0';
		spprintf(&ret, 0, "%s/%s", entry->filename, entry->link);
		return ret;
	}
	return entry->link;
}