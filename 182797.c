dns_zone_setalsonotifywithkeys(dns_zone_t *zone, const isc_sockaddr_t *notify,
			       dns_name_t **keynames, uint32_t count)
{
	return (dns_zone_setalsonotifydscpkeys(zone, notify, NULL, keynames,
					       count));
}