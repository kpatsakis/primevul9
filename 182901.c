add_signing_records(dns_db_t *db, dns_rdatatype_t privatetype,
		    dns_dbversion_t *ver, dns_diff_t *diff,
		    bool sign_all)
{
	dns_difftuple_t *tuple, *newtuple = NULL;
	dns_rdata_dnskey_t dnskey;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	bool flag;
	isc_region_t r;
	isc_result_t result = ISC_R_SUCCESS;
	uint16_t keyid;
	unsigned char buf[5];
	dns_name_t *name = dns_db_origin(db);

	for (tuple = ISC_LIST_HEAD(diff->tuples);
	     tuple != NULL;
	     tuple = ISC_LIST_NEXT(tuple, link)) {
		if (tuple->rdata.type != dns_rdatatype_dnskey)
			continue;

		result = dns_rdata_tostruct(&tuple->rdata, &dnskey, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		if ((dnskey.flags &
		     (DNS_KEYFLAG_OWNERMASK|DNS_KEYTYPE_NOAUTH))
			 != DNS_KEYOWNER_ZONE)
			continue;

		dns_rdata_toregion(&tuple->rdata, &r);

		keyid = dst_region_computeid(&r);

		buf[0] = dnskey.algorithm;
		buf[1] = (keyid & 0xff00) >> 8;
		buf[2] = (keyid & 0xff);
		buf[3] = (tuple->op == DNS_DIFFOP_ADD) ? 0 : 1;
		buf[4] = 0;
		rdata.data = buf;
		rdata.length = sizeof(buf);
		rdata.type = privatetype;
		rdata.rdclass = tuple->rdata.rdclass;

		if (sign_all || tuple->op == DNS_DIFFOP_DEL) {
			CHECK(rr_exists(db, ver, name, &rdata, &flag));
			if (flag)
				continue;
			CHECK(dns_difftuple_create(diff->mctx, DNS_DIFFOP_ADD,
						   name, 0, &rdata, &newtuple));
			CHECK(do_one_tuple(&newtuple, db, ver, diff));
			INSIST(newtuple == NULL);
		}

		/*
		 * Remove any record which says this operation has already
		 * completed.
		 */
		buf[4] = 1;
		CHECK(rr_exists(db, ver, name, &rdata, &flag));
		if (flag) {
			CHECK(dns_difftuple_create(diff->mctx, DNS_DIFFOP_DEL,
						   name, 0, &rdata, &newtuple));
			CHECK(do_one_tuple(&newtuple, db, ver, diff));
			INSIST(newtuple == NULL);
		}
	}
 failure:
	return (result);
}