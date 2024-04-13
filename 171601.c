ldns_pkt_tsig_sign(ldns_pkt *pkt, const char *key_name, const char *key_data,
	uint16_t fudge, const char *algorithm_name, const ldns_rdf *query_mac)
{
	return ldns_pkt_tsig_sign_next(pkt, key_name, key_data, fudge, algorithm_name, query_mac, 0);
}