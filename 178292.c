static void refresh_dns_entry(struct cache_entry *entry, char *name)
{
	int age = 1;

	if (!ipv4_resolve) {
		ipv4_resolve = g_resolv_new(0);
		g_resolv_set_address_family(ipv4_resolve, AF_INET);
		g_resolv_add_nameserver(ipv4_resolve, "127.0.0.1", 53, 0);
	}

	if (!ipv6_resolve) {
		ipv6_resolve = g_resolv_new(0);
		g_resolv_set_address_family(ipv6_resolve, AF_INET6);
		g_resolv_add_nameserver(ipv6_resolve, "::1", 53, 0);
	}

	if (!entry->ipv4) {
		debug("Refreshing A record for %s", name);
		g_resolv_lookup_hostname(ipv4_resolve, name,
					dummy_resolve_func, NULL);
		age = 4;
	}

	if (!entry->ipv6) {
		debug("Refreshing AAAA record for %s", name);
		g_resolv_lookup_hostname(ipv6_resolve, name,
					dummy_resolve_func, NULL);
		age = 4;
	}

	entry->hits -= age;
	if (entry->hits < 0)
		entry->hits = 0;
}