addifmissing(dns_keytable_t *keytable, dns_keynode_t *keynode, void *arg) {
	dns_db_t *db = ((struct addifmissing_arg *)arg)->db;
	dns_dbversion_t *ver = ((struct addifmissing_arg *)arg)->ver;
	dns_diff_t *diff = ((struct addifmissing_arg *)arg)->diff;
	dns_zone_t *zone = ((struct addifmissing_arg *)arg)->zone;
	bool *changed = ((struct addifmissing_arg *)arg)->changed;
	isc_result_t result;
	dns_keynode_t *dummy = NULL;

	if (((struct addifmissing_arg *)arg)->result != ISC_R_SUCCESS)
		return;

	if (dns_keynode_managed(keynode)) {
		dns_fixedname_t fname;
		dns_name_t *keyname;
		dst_key_t *key;

		key = dns_keynode_key(keynode);
		if (key == NULL)
			return;
		dns_fixedname_init(&fname);

		keyname = dst_key_name(key);
		result = dns_db_find(db, keyname, ver,
				     dns_rdatatype_keydata,
				     DNS_DBFIND_NOWILD, 0, NULL,
				     dns_fixedname_name(&fname),
				     NULL, NULL);
		if (result == ISC_R_SUCCESS)
			return;
		dns_keytable_attachkeynode(keytable, keynode, &dummy);
		result = create_keydata(zone, db, ver, diff, keytable,
					&dummy, changed);
		if (result != ISC_R_SUCCESS && result != ISC_R_NOMORE)
			((struct addifmissing_arg *)arg)->result = result;
	}
};