dns_zone_setmasters(dns_zone_t *zone, const isc_sockaddr_t *masters,
		    uint32_t count)
{
	isc_result_t result;

	result = dns_zone_setmasterswithkeys(zone, masters, NULL, count);
	return (result);
}