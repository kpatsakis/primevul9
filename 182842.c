zone_setrawdata(dns_zone_t *zone, dns_masterrawheader_t *header) {
	if ((header->flags & DNS_MASTERRAW_SOURCESERIALSET) == 0)
		return;

	zone->sourceserial = header->sourceserial;
	zone->sourceserialset = true;
}