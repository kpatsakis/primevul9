create_keydata(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *ver,
	       dns_diff_t *diff, dns_keytable_t *keytable,
	       dns_keynode_t **keynodep, bool *changed)
{
	const char me[] = "create_keydata";
	isc_result_t result = ISC_R_SUCCESS;
	isc_buffer_t keyb, dstb;
	unsigned char key_buf[4096], dst_buf[DST_KEY_MAXSIZE];
	dns_rdata_keydata_t keydata;
	dns_rdata_dnskey_t dnskey;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_keynode_t *keynode;
	isc_stdtime_t now;
	isc_region_t r;
	dst_key_t *key;

	REQUIRE(keynodep != NULL);
	keynode = *keynodep;

	ENTER;
	isc_stdtime_get(&now);

	/* Loop in case there's more than one key. */
	while (result == ISC_R_SUCCESS) {
		dns_keynode_t *nextnode = NULL;

		key = dns_keynode_key(keynode);
		if (key == NULL)
			goto skip;

		isc_buffer_init(&dstb, dst_buf, sizeof(dst_buf));
		CHECK(dst_key_todns(key, &dstb));

		/* Convert DST key to DNSKEY. */
		dns_rdata_reset(&rdata);
		isc_buffer_usedregion(&dstb, &r);
		dns_rdata_fromregion(&rdata, dst_key_class(key),
				     dns_rdatatype_dnskey, &r);

		/* DSTKEY to KEYDATA. */
		CHECK(dns_rdata_tostruct(&rdata, &dnskey, NULL));
		CHECK(dns_keydata_fromdnskey(&keydata, &dnskey, now, 0, 0,
					     NULL));

		/* KEYDATA to rdata. */
		dns_rdata_reset(&rdata);
		isc_buffer_init(&keyb, key_buf, sizeof(key_buf));
		CHECK(dns_rdata_fromstruct(&rdata,
					   zone->rdclass, dns_rdatatype_keydata,
					   &keydata, &keyb));

		/* Add rdata to zone. */
		CHECK(update_one_rr(db, ver, diff, DNS_DIFFOP_ADD,
				    dst_key_name(key), 0, &rdata));
		*changed = true;

		/* Refresh new keys from the zone apex as soon as possible. */
		set_refreshkeytimer(zone, &keydata, now, true);

 skip:
		result = dns_keytable_nextkeynode(keytable, keynode, &nextnode);
		if (result != ISC_R_NOTFOUND) {
			dns_keytable_detachkeynode(keytable, &keynode);
			keynode = nextnode;
		}
	}

	if (keynode != NULL)
		dns_keytable_detachkeynode(keytable, &keynode);
	*keynodep = NULL;

	return (ISC_R_SUCCESS);

  failure:
	return (result);
}