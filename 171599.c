ldns_pkt_tsig_verify(ldns_pkt *pkt, const uint8_t *wire, size_t wirelen, const char *key_name,
	const char *key_data, const ldns_rdf *orig_mac_rdf)
{
	return ldns_pkt_tsig_verify_next(pkt, wire, wirelen, key_name, key_data, orig_mac_rdf, 0);
}