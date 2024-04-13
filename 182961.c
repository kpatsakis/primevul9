dns_zone_dumptostream(dns_zone_t *zone, FILE *fd, dns_masterformat_t format,
		      const dns_master_style_t *style,
		      const uint32_t rawversion)
{
	return (dumptostream(zone, fd, style, format, rawversion));
}