phar_entry_info *phar_get_link_source(phar_entry_info *entry) /* {{{ */
{
	phar_entry_info *link_entry;
	char *link;

	if (!entry->link) {
		return entry;
	}

	link = phar_get_link_location(entry);
	if (NULL != (link_entry = zend_hash_str_find_ptr(&(entry->phar->manifest), entry->link, strlen(entry->link))) ||
		NULL != (link_entry = zend_hash_str_find_ptr(&(entry->phar->manifest), link, strlen(link)))) {
		if (link != entry->link) {
			efree(link);
		}
		return phar_get_link_source(link_entry);
	} else {
		if (link != entry->link) {
			efree(link);
		}
		return NULL;
	}
}