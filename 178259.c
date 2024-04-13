static void cache_refresh_entry(struct cache_entry *entry)
{

	cache_enforce_validity(entry);

	if (entry->hits > 2 && !entry->ipv4)
		entry->want_refresh = true;
	if (entry->hits > 2 && !entry->ipv6)
		entry->want_refresh = true;

	if (entry->want_refresh) {
		char *c;
		char dns_name[NS_MAXDNAME + 1];
		entry->want_refresh = false;

		/* turn a DNS name into a hostname with dots */
		strncpy(dns_name, entry->key, NS_MAXDNAME);
		c = dns_name;
		while (c && *c) {
			int jump;
			jump = *c;
			*c = '.';
			c += jump + 1;
		}
		debug("Refreshing %s\n", dns_name);
		/* then refresh the hostname */
		refresh_dns_entry(entry, &dns_name[1]);
	}
}