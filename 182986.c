dns_zone_setalsonotify(dns_zone_t *zone, const isc_sockaddr_t *notify,
		       uint32_t count)
{
	return (dns_zone_setalsonotifydscpkeys(zone, notify, NULL, NULL,
					       count));
}