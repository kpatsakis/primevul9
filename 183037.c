clear_keylist(dns_dnsseckeylist_t *list, isc_mem_t *mctx) {
	dns_dnsseckey_t *key;
	while (!ISC_LIST_EMPTY(*list)) {
		key = ISC_LIST_HEAD(*list);
		ISC_LIST_UNLINK(*list, key, link);
		dns_dnsseckey_destroy(mctx, &key);
	}
}