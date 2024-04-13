dnsc_find_cert(struct dnsc_env* dnscenv, struct sldns_buffer* buffer)
{
	const dnsccert *certs = dnscenv->certs;
	struct dnscrypt_query_header *dnscrypt_header;
	size_t i;

	if (sldns_buffer_limit(buffer) < DNSCRYPT_QUERY_HEADER_SIZE) {
		return NULL;
	}
	dnscrypt_header = (struct dnscrypt_query_header *)sldns_buffer_begin(buffer);
	for (i = 0U; i < dnscenv->signed_certs_count; i++) {
		if (memcmp(certs[i].magic_query, dnscrypt_header->magic_query,
                   DNSCRYPT_MAGIC_HEADER_LEN) == 0) {
			return &certs[i];
		}
	}
	return NULL;
}